#include "timer.h"


uint64_t repeat = 0;

static void callback(uv_timer_t *handle)
{
    repeat = repeat + 1;
    printf("repeat count:%d \n", repeat);
}

PHP_FUNCTION(lib_timer_test)
        {
                uv_loop_t *loop = uv_default_loop();
                uv_timer_t timer_req;
                uv_timer_init(loop, &timer_req);

                uv_timer_start(&timer_req, callback, 1000, 1000);
                uv_run(loop, UV_RUN_DEFAULT);
        }
