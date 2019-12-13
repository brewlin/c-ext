#include "../include/lib_shamem.h"
//采用 mmap 方式申请共享内存
#if (HAVE_MAP_ANON)

int shm_alloc(shm_t *shm)
{
    shm->addr = (u_char *) mmap(NULL, shm->size,
                                PROT_READ|PROT_WRITE,
                                MAP_ANON|MAP_SHARED, -1, 0);

    if (shm->addr == MAP_FAILED) {
        php_printf( "mmap(MAP_ANON|MAP_SHARED, %d) failed", shm->size);
        return ERROR;
    }
    php_printf( "mmap(MAP_ANON|MAP_SHARED, %d) success", shm->size);
    return OK;
}


void shm_free(shm_t *shm)
{
    if (munmap((void *) shm->addr, shm->size) == -1) {
        php_printf( "munmap(%s, %d) failed", shm->addr, shm->size);
    }
    php_printf( "munmap( %d) success",  shm->size);
}
//采用 文件映射方式申请共享内存
#elif (HAVE_MAP_DEVZERO)

int_t shm_alloc(shm_t *shm)
{
    fd_t  fd;

    fd = open("/dev/zero", O_RDWR);

    if (fd == -1) {
        php_printf( "open(\"/dev/zero\") failed");
        return ERROR;
    }

    shm->addr = (u_char *) mmap(NULL, shm->size, PROT_READ|PROT_WRITE,
                                MAP_SHARED, fd, 0);

    if (shm->addr == MAP_FAILED) {
        php_printf( "mmap(/dev/zero, MAP_SHARED, %d) failed", shm->size);
    }

    if (close(fd) == -1) {
        php_printf("close(\"/dev/zero\") failed");
    }

    return (shm->addr == MAP_FAILED) ? ERROR : OK;
}


void shm_free(shm_t *shm)
{
    if (munmap((void *) shm->addr, shm->size) == -1) {
        php_printf("munmap(%s, %d) failed", shm->addr, shm->size);
    }
}
//采用 shmget 系统调用方式申请共享内存
#elif (HAVE_SYSVSHM)

#include <sys/ipc.h>
#include <sys/shm.h>


int_t shm_alloc(shm_t *shm)
{
    int  id;

    id = shmget(IPC_PRIVATE, shm->size, (SHM_R|SHM_W|IPC_CREAT));

    if (id == -1) {
        php_printf("shmget(%d) failed", shm->size);
        return ERROR;
    }

    php_printf(, "shmget id: %d", id);

    shm->addr = shmat(id, NULL, 0);

    if (shm->addr == (void *) -1) {
        php_printf( "shmat() failed");
    }

    if (shmctl(id, IPC_RMID, NULL) == -1) {
        php_printf("shmctl(IPC_RMID) failed");
    }

    return (shm->addr == (void *) -1) ? ERROR : OK;
}


void shm_free(shm_t *shm)
{
    if (shmdt(shm->addr) == -1) {
        php_printf("shmdt(%s) failed", shm->addr);
    }
}

#endif
