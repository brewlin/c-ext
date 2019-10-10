#include "lib_coroutine.h"

using Lib::PHPCoroutine;

php_coro_task PHPCoroutine::main_task = {0}
php_coro_task PHPCoroutine::get_task()
{
    php_coro_task *task = (php_coro_task *)Coroutine::get_current_task();
    return task ? task : &main_task;
}

long PHPCoroutine::create(zend_fcall_info_cache *fci_cache,uint32_t argc,zval *argv){
    php_coro_args php_coro_args;

    php_coro_args.fci_cache = fci_cache;
    php_coro_args.argv = argv;
    php_coro_args.argc = argc;
    save_task(get_task());
    return Coroutine::create(create_func,(void *)&php_coro_args);

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
//获取当前协程栈内容
php_coro_task* PHPCoroutine::get_task()
{
    return nullptr;
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
    call = (zend_execute_data *)EG(vm_stack_top);
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
    //把当前协程栈信息保存到task里面
    if(func->type == ZEND_USER_FUNCTION){
        ZVAL_UNDEF(retval);
        EG(current_execute_data) = NULL;
        zend_init_func_execute_data(call,&func->op_array,retval);
        zend_execute_ex(EG(current_execute_daa));
    }
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

long Coroutine::create(coroutine_func_t fn,void *args)
{
    return (new Coroutine(fn,args))->run();
}