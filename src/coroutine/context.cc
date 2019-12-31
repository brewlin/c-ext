#include "context.h"
#include "lib.h"

using namespace lib;

Context::Context(size_t stack_size, coroutine_func_t fn, void* private_data) :
        fn_(fn), stack_size_(stack_size), private_data_(private_data)
{
    swap_ctx_ = nullptr;

    stack_ = (char*) malloc(stack_size_);
    
    void* sp = (void*) ((char*) stack_ + stack_size_);
    ctx_ = make_fcontext(sp, stack_size_, (void (*)(intptr_t))&context_func);
}

void Context::context_func(void *arg){
    Context *_this = (Context *)arg;
    _this->fn_(_this->private_data_);
    _this->end_ = true;
    _this->swap_out();
}
//让出携程上下文
bool Context::swap_out()
{
    jump_fcontext(&ctx_,swap_ctx_,(intptr_t)this,true);
    return true;
}
//保存上下问到ofc，加载nfc上下文
bool Context::swap_in()
{
    jump_fcontext(&swap_ctx_,ctx_,(intptr_t)this,true);
    return true;
}
Context::~Context()
{
    if (swap_ctx_)
    {
        free(stack_);
        stack_ = NULL;
    }
}