//
// Created by root on 2019/12/18.
//

#ifndef C_EXT_AUTOMIC_H
#define C_EXT_AUTOMIC_H



#if (HAVE_LIBATOMIC)

#define AO_REQUIRE_CAS
#include <atomic_ops.h>

#define HAVE_ATOMIC_OPS  1

typedef long                        atomic_int_t;
typedef AO_t                        atomic_uint_t;
typedef volatile atomic_uint_t  atomic_t;

#if (PTR_SIZE == 8)
#define ATOMIC_T_LEN            (sizeof("-9223372036854775808") - 1)
#else
#define ATOMIC_T_LEN            (sizeof("-2147483648") - 1)
#endif

#define atomic_cmp_set(lock, old, new)                                    \
    AO_compare_and_swap(lock, old, new)
#define atomic_fetch_add(value, add)                                      \
    AO_fetch_and_add(value, add)
#define memory_barrier()        AO_nop()
#define cpu_pause()


#elif (DARWIN_ATOMIC)

/*
 * use Darwin 8 atomic(3) and barrier(3) operations
 * optimized at run-time for UP and SMP
 */

#include <libkern/OSAtomic.h>

/* "bool" conflicts with perl's CORE/handy.h */
#undef bool


#define HAVE_ATOMIC_OPS  1

#if (PTR_SIZE == 8)

typedef int64_t                     atomic_int_t;
typedef uint64_t                    atomic_uint_t;
#define ATOMIC_T_LEN            (sizeof("-9223372036854775808") - 1)

#define atomic_cmp_set(lock, old, new)                                    \
    OSAtomicCompareAndSwap64Barrier(old, new, (int64_t *) lock)

#define atomic_fetch_add(value, add)                                      \
    (OSAtomicAdd64(add, (int64_t *) value) - add)

#else

typedef int32_t                     atomic_int_t;
typedef uint32_t                    atomic_uint_t;
#define ATOMIC_T_LEN            (sizeof("-2147483648") - 1)

#define atomic_cmp_set(lock, old, new)                                    \
    OSAtomicCompareAndSwap32Barrier(old, new, (int32_t *) lock)

#define atomic_fetch_add(value, add)                                      \
    (OSAtomicAdd32(add, (int32_t *) value) - add)

#endif

#define memory_barrier()        OSMemoryBarrier()

#define cpu_pause()

typedef volatile atomic_uint_t  atomic_t;


#elif (HAVE_GCC_ATOMIC)

/* GCC 4.1 builtin atomic operations */

#define HAVE_ATOMIC_OPS  1

typedef long                        atomic_int_t;
typedef unsigned long               atomic_uint_t;

#if (PTR_SIZE == 8)
#define ATOMIC_T_LEN            (sizeof("-9223372036854775808") - 1)
#else
#define ATOMIC_T_LEN            (sizeof("-2147483648") - 1)
#endif

typedef volatile atomic_uint_t  atomic_t;


#define atomic_cmp_set(lock, old, set)                                    \
    __sync_bool_compare_and_swap(lock, old, set)

#define atomic_fetch_add(value, add)                                      \
    __sync_fetch_and_add(value, add)

#define memory_barrier()        __sync_synchronize()

#if ( __i386__ || __i386 || __amd64__ || __amd64 )
#define cpu_pause()             __asm__ ("pause")
#else
#define cpu_pause()
#endif


#elif ( __i386__ || __i386 )

typedef int32_t                     atomic_int_t;
typedef uint32_t                    atomic_uint_t;
typedef volatile atomic_uint_t  atomic_t;
#define ATOMIC_T_LEN            (sizeof("-2147483648") - 1)


#if ( __SUNPRO_C )

#define HAVE_ATOMIC_OPS  1

atomic_uint_t
atomic_cmp_set(atomic_t *lock, atomic_uint_t old,
    atomic_uint_t set);

atomic_int_t
atomic_fetch_add(atomic_t *value, atomic_int_t add);

/*
 * Sun Studio 12 exits with segmentation fault on '__asm ("pause")',
 * so cpu_pause is declared in src/os/unix/sunpro_x86.il
 */

void
cpu_pause(void);

/* the code in src/os/unix/sunpro_x86.il */

#define memory_barrier()        __asm (".volatile"); __asm (".nonvolatile")


#else /* ( __GNUC__ || __INTEL_COMPILER ) */

#define HAVE_ATOMIC_OPS  1

#include "gcc_atomic_x86.h"

#endif


#elif ( __amd64__ || __amd64 )

typedef int64_t                     atomic_int_t;
typedef uint64_t                    atomic_uint_t;
typedef volatile atomic_uint_t  atomic_t;
#define ATOMIC_T_LEN            (sizeof("-9223372036854775808") - 1)


#if ( __SUNPRO_C )

#define HAVE_ATOMIC_OPS  1

atomic_uint_t
atomic_cmp_set(atomic_t *lock, atomic_uint_t old,
    atomic_uint_t set);

atomic_int_t
atomic_fetch_add(atomic_t *value, atomic_int_t add);

/*
 * Sun Studio 12 exits with segmentation fault on '__asm ("pause")',
 * so cpu_pause is declared in src/os/unix/sunpro_amd64.il
 */

void
cpu_pause(void);

/* the code in src/os/unix/sunpro_amd64.il */

#define memory_barrier()        __asm (".volatile"); __asm (".nonvolatile")


#else /* ( __GNUC__ || __INTEL_COMPILER ) */

#define HAVE_ATOMIC_OPS  1

// #include "gcc_atomic_amd64.h"

#endif


#elif ( __sparc__ || __sparc || __sparcv9 )

#if (PTR_SIZE == 8)

typedef int64_t                     atomic_int_t;
typedef uint64_t                    atomic_uint_t;
#define ATOMIC_T_LEN            (sizeof("-9223372036854775808") - 1)

#else

typedef int32_t                     atomic_int_t;
typedef uint32_t                    atomic_uint_t;
#define ATOMIC_T_LEN            (sizeof("-2147483648") - 1)

#endif

typedef volatile atomic_uint_t  atomic_t;


#if ( __SUNPRO_C )

#define HAVE_ATOMIC_OPS  1

#include "sunpro_atomic_sparc64.h"


#else /* ( __GNUC__ || __INTEL_COMPILER ) */

#define HAVE_ATOMIC_OPS  1

#include "gcc_atomic_sparc64.h"

#endif


#elif ( __powerpc__ || __POWERPC__ )

#define HAVE_ATOMIC_OPS  1

#if (PTR_SIZE == 8)

typedef int64_t                     atomic_int_t;
typedef uint64_t                    atomic_uint_t;
#define ATOMIC_T_LEN            (sizeof("-9223372036854775808") - 1)

#else

typedef int32_t                     atomic_int_t;
typedef uint32_t                    atomic_uint_t;
#define ATOMIC_T_LEN            (sizeof("-2147483648") - 1)

#endif

typedef volatile atomic_uint_t  atomic_t;


#include "gcc_atomic_ppc.h"

#endif


//当cpu原生架构不支持等时候 在c语言上模拟原子操作
// #if !(HAVE_ATOMIC_OPS)
#include "lib.h"

#define HAVE_ATOMIC_OPS  0


typedef int32_t                     atomic_int_t;
typedef uint32_t                    atomic_uint_t;
typedef volatile atomic_uint_t      atomic_t;

#define ATOMIC_T_LEN            (sizeof("-2147483648") - 1)


static inline atomic_uint_t 
atomic_cmp_set(atomic_t *lock, atomic_uint_t old,atomic_uint_t set)
{
     if (*lock == old) {
         *lock = set;
         return 1;
     }

     return 0;
}


static inline atomic_int_t 
atomic_fetch_add(atomic_t *value, atomic_int_t add)
{
     atomic_int_t  old;

     old = *value;
     *value += add;

     return old;
}

#define memory_barrier()
#define cpu_pause()

// #endif

// 自旋锁实现
void spinlock(atomic_t *lock, atomic_int_t value, uint_t spin);
// 尝试获取锁，如果失败直接返回
#define trylock(lock)  (*(lock) == 0 && atomic_cmp_set(lock, 0, 1))
//阻塞方式解除锁
#define unlock(lock)    *(lock) = 0


#endif //C_EXT_AUTOMIC_H
