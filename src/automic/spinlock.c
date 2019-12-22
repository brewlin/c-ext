#include "automic.h"

//自旋锁实现
void
spinlock(atomic_t *lock, atomic_int_t value, uint_t spin)
{

// #if (NGX_HAVE_ATOMIC_OPS)

    uint_t  i, n;

    for ( ;; ) {
        //这里的lock 变量为共享内存变量， 等待其他进程更新锁
        if (*lock == 0 && atomic_cmp_set(lock, 0, value)) {
            return;
        }

        if (ncpu > 1) {

            for (n = 1; n < spin; n <<= 1) {

                for (i = 0; i < n; i++) {
                    cpu_pause();
                }

                if (*lock == 0 && atomic_cmp_set(lock, 0, value)) {
                    return;
                }
            }
        }
        //让出cpu 调度 
        sched_yield();
    }

// #else

#if (THREADS)

#error spinlock() or atomic_cmp_set() are not defined !

#endif

// #endif

}
