#include "lib_coroutine.h"
#include "coroutine.h"
#include <iostream>
#include "lib.h"

using lib::PHPCoroutine;
using lib::Coroutine;

php_coro_task PHPCoroutine::main_task = {0};
php_coro_task* PHPCoroutine::get_task()
{
    php_coro_task *task = (php_coro_task *)Coroutine::get_current_task();
    return task ? task : &main_task;
}

long PHPCoroutine::create(zend_fcall_info_cache *fci_cache, uint32_t argc, zval *argv)
{
    php_coro_args php_coro_args;
    php_coro_args.fci_cache = fci_cache;
    php_coro_args.argv = argv;
    php_coro_args.argc = argc;
    save_task(get_task());

    return Coroutine::create(create_func,(void*) &php_coro_args);
}
void PHPCoroutine::save_task(php_coro_task *task)
{
    save_vm_stack(task);
}
//保存协程栈内容
void PHPCoroutine::save_vm_stack(php_coro_task *task)
{
    task->vm_stack_top = EG(vm_stack_top);
    task->vm_stack_end = EG(vm_stack_end);
    task->vm_stack = EG(vm_stack);
     task->vm_stack_page_size = EG(vm_stack_page_size);
    task->execute_data = EG(current_execute_data);
}


//实现创建协程方法
void PHPCoroutine::create_func(void *arg)
{
    int i;
    php_coro_args *php_arg = (php_coro_args *)arg;

    zend_fcall_info_cache fci_cache = *php_arg->fci_cache;
    zend_function *func = fci_cache.function_handler;

    zval *argv = php_arg->argv;
    int argc = php_arg->argc;
    php_coro_task *task;
    zend_execute_data *call;
    zval _retval,*retval = &_retval;

    //获取一个新的php栈
    vm_stack_init();
    call = (zend_execute_data *)(EG(vm_stack_top));
    task = (php_coro_task *) EG(vm_stack_top);
    EG(vm_stack_top) = (zval *)((char *)call + PHP_CORO_TASK_SLOT * sizeof(zval));



    call = zend_vm_stack_push_call_frame(
        ZEND_CALL_TOP_FUNCTION | ZEND_CALL_ALLOCATED,
        func,argc,fci_cache.called_scope,fci_cache.object
    );
    //拷贝用户传入的参数
    for(i = 0; i < argc; ++ i ){
        zval *param;
        zval *arg = &argv[i];
        param = ZEND_CALL_ARG(call,i + 1);
        ZVAL_COPY(param,arg);
    }
    call->symbol_table = NULL;
    EG(current_execute_data) = call;
    save_vm_stack(task);
    task->co = Coroutine::get_current();
    task->co->set_task((void *)task);
    task->defer_tasks = nullptr;

    //把当前协程栈信息保存到task里面
    if(func->type == ZEND_USER_FUNCTION){
        ZVAL_UNDEF(retval);
        EG(current_execute_data) = NULL;
         zend_init_func_execute_data(call,&func->op_array,retval);
        zend_execute_ex(EG(current_execute_data));
    }
    task = get_task();
    std::stack<php_lib_fci_fcc *> *defer_tasks = task->defer_tasks;

    if (defer_tasks) {
        php_lib_fci_fcc *defer_fci_fcc;
        zval result;
        while(!defer_tasks->empty())
        {
            defer_fci_fcc = defer_tasks->top();
            defer_tasks->pop();
            defer_fci_fcc->fci.retval = &result;

            if (zend_call_function(&defer_fci_fcc->fci, &defer_fci_fcc->fcc) != SUCCESS)
            {
                php_error_docref(NULL, E_WARNING, "defer execute error");
                return;
            }
            efree(defer_fci_fcc);
        }
        delete defer_tasks;
        task->defer_tasks = nullptr;
    }
    zend_vm_stack stack = EG(vm_stack);
    efree(stack);
    zval_ptr_dtor(retval);

}
void PHPCoroutine::vm_stack_init(void)
{
    uint32_t size = DEFAULT_PHP_STACK_PAGE_SIZE;
    //从堆上申请内存  来模拟栈
    zend_vm_stack page = (zend_vm_stack)emalloc(size);

    //栈顶
    page->top = ZEND_VM_STACK_ELEMENTS(page);
    //end 用来表示栈的边界
    page->end = (zval *)((char *)page + size);
    page->prev = NULL;

    //表示修改现在的php栈，指向我们申请的堆栈
    EG(vm_stack) = page;
    EG(vm_stack)->top ++ ;
    EG(vm_stack_top) = EG(vm_stack)->top;
    EG(vm_stack_end) = EG(vm_stack)->end;
     EG(vm_stack_page_size) = size;
}

void PHPCoroutine::defer(php_lib_fci_fcc *defer_fci_fcc)
{
    php_coro_task *task = (php_coro_task *)get_task();
    if(task->defer_tasks == nullptr)
    {
        task->defer_tasks = new std::stack<php_lib_fci_fcc *>;
    }
    task->defer_tasks->push(defer_fci_fcc);
}
int PHPCoroutine::sleep(double seconds)
{
    Coroutine::sleep(seconds);
    return 0;
}

void PHPCoroutine::on_yield(void *arg)
{
    php_coro_task *task = (php_coro_task *) arg;
    php_coro_task *origin_task = get_origin_task(task);
    save_task(task);
    restore_task(origin_task);
}

void PHPCoroutine::on_resume(void *arg)
{
    php_coro_task *task = (php_coro_task *) arg;
    php_coro_task *current_task = get_task();
    save_task(current_task);
    restore_task(task);
}

/**
 * load PHP stack
 */
void PHPCoroutine::restore_task(php_coro_task *task)
{
    restore_vm_stack(task);
}

/**
 * load PHP stack
 */
inline void PHPCoroutine::restore_vm_stack(php_coro_task *task)
{
    EG(vm_stack_top) = task->vm_stack_top;
    EG(vm_stack_end) = task->vm_stack_end;
    EG(vm_stack) = task->vm_stack;
    EG(vm_stack_page_size) = task->vm_stack_page_size;
    EG(current_execute_data) = task->execute_data;
}

void PHPCoroutine::init()
{
    Coroutine::set_on_yield(on_yield);
    Coroutine::set_on_resume(on_resume);
}
