//
// Created by Administrator on 2019/12/19 0019.
//

#ifndef C_EXT_PROCESS_H
#define C_EXT_PROCESS_H

#include "lib.h"
#include "php_lib.h"

typedef void (*spawn_proc_pt) (long cid);


typedef struct {
   pid_t pid;
   int status;

   //int channel[2]
   socket_t channel[2];

   //派生的子进程回调函数，创建子进程后会调用该函数
   spawn_proc_pt proc;
   void *data;
   char *name;

    unsigned            respawn:1;
    unsigned            just_spawn:1;
    unsigned            detached:1;
    unsigned            exiting:1;
    unsigned            exited:1;

}process_t;

typedef struct {
    zend_fcall_info fci;
    zend_fcall_info_cache fcc;
    zval *obj;
}php_lib_fun;

#define MAX_PROCESSES         1024
#define INVALID_PID  -1

#define PROCESS_NORESPAWN     -1
#define PROCESS_JUST_SPAWN    -2
#define PROCESS_RESPAWN       -3
#define PROCESS_JUST_RESPAWN  -4
#define PROCESS_DETACHED      -5

//定义全局pid
extern pid_t      ce_pid;
extern pid_t      ce_parent;
extern process_t  processes[MAX_PROCESSES];

pid_t spwan_process(spawn_proc_pt proc,void *data,char *name,int_t respawn);

#endif //C_EXT_PROCESS_H
