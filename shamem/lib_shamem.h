#ifndef _SHMEM_H_INCLUDED_
#define _SHMEM_H_INCLUDED_


#define  OK          0
#define  ERROR      -1


#include <sys/mman.h>
#include "php.h"
#include "ext/standard/info.h"
#include "php_shamem.h"
typedef unsigned char u_char;

typedef struct {
    u_char      *addr;
    size_t       size;
    u_char    name;
} shm_t;


int shm_alloc(shm_t *shm);
void shm_free(shm_t *shm);


#endif /* _SHMEM_H_INCLUDED_ */
