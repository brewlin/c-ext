#ifndef _SHMEM_H_INCLUDED_
#define _SHMEM_H_INCLUDED_

#include "php_lib.h"

#define  OK          0
#define  ERROR      -1

typedef unsigned char u_char;
#define HAVE_MAP_ANON 1

typedef struct {
    u_char      *addr;
    size_t       size;
    u_char    name;
} shm_t;


int shm_alloc(shm_t *shm);
void shm_free(shm_t *shm);


#endif /* _SHMEM_H_INCLUDED_ */
