//
// Created by Administrator on 2019/12/19 0019.
//

#include "process.h"
#include "channel.h"

//保存所有自定义进程
process_t processes[MAX_PROCESSES];
socket_t     channel;

pid_t      ce_pid;
pid_t      ce_parent;

int_t        last_process;
//当前进程的编号
int_t        process_slot;

pid_t spwan_process(spawn_proc_pt proc,void *data,char *name,uint_t respawn)
{
    unsigned long     on;
    pid_t  pid;
    uint_t  s;

    if (respawn >= 0) {
        s = respawn;

    } else {
        for (s = 0; s < last_process; s++) {
            //找到最后一个进程所在的位置
            if (processes[s].pid == -1) {
                break;
            }
        }
        //如果 达到了最大的创建进程数，则返回错误
        if (s == MAX_PROCESSES) {
            php_printf("no more than %d processes can be spawned",MAX_PROCESSES);
            return INVALID_PID;
        }
    }


    if (respawn != PROCESS_DETACHED) {

        /* Solaris 9 still has no AF_LOCAL */
        //创建进程间通信，nginx主要是用来传递int型， 也就是主进程向子进程发送信号进行管理
        //创建成功后
        //processes[s].channel[0] 主进程读写
        //processes[s].channel[1] 子进程读写
        if (socketpair(AF_UNIX, SOCK_STREAM, 0, processes[s].channel) == -1)
        {
            php_printf("socketpair() failed while spawning \"%s\"", name);
            return INVALID_PID;
        }

        php_printf("channel %d:%d",processes[s].channel[0],processes[s].channel[1]);

//        //将unix socket 套接字设置为非阻塞
//        if (nonblocking(processes[s].channel[0]) == -1) {
//            log_error(LOG_ALERT, cycle->log, errno,
//                          nonblocking_n " failed while spawning \"%s\"",
//                    name);
//            close_channel(processes[s].channel, cycle->log);
//            return INVALID_PID;
//        }
//        //将子进程的 channel 管道 设为非阻塞
//        if (nonblocking(processes[s].channel[1]) == -1) {
//            log_error(LOG_ALERT, cycle->log, errno,
//                          nonblocking_n " failed while spawning \"%s\"",
//                    name);
//            close_channel(processes[s].channel, cycle->log);
//            return INVALID_PID;
//        }

        on = 1;
//        if (ioctl(processes[s].channel[0], FIOASYNC, &on) == -1) {
//            log_error(LOG_ALERT, cycle->log, errno,
//                          "ioctl(FIOASYNC) failed while spawning \"%s\"", name);
//            close_channel(processes[s].channel, cycle->log);
//            return INVALID_PID;
//        }
//
//        if (fcntl(processes[s].channel[0], F_SETOWN, pid) == -1) {
//            log_error(LOG_ALERT, cycle->log, errno,
//                          "fcntl(F_SETOWN) failed while spawning \"%s\"", name);
//            close_channel(processes[s].channel, cycle->log);
//            return INVALID_PID;
//        }

        if (fcntl(processes[s].channel[0], F_SETFD, FD_CLOEXEC) == -1) {
                php_printf("fcntl(FD_CLOEXEC) failed while spawning \"%s\"",name);
            close_channel(processes[s].channel);
            return INVALID_PID;
        }

        if (fcntl(processes[s].channel[1], F_SETFD, FD_CLOEXEC) == -1) {
                php_printf("fcntl(FD_CLOEXEC) failed while spawning \"%s\"", name);
            close_channel(processes[s].channel);
            return INVALID_PID;
        }

        channel = processes[s].channel[1];

    } else {
        processes[s].channel[0] = -1;
        processes[s].channel[1] = -1;
    }

    process_slot = s;

    //创建子进程
    pid = fork();

    switch (pid) {

        case -1:
            php_printf("fork() failed while spawning \"%s\"", name);
            close_channel(processes[s].channel);
            return INVALID_PID;
            //子进程
        case 0:
            //此时fork 返回的是0 把主进程的pid 赋值给parent
            ce_parent = ce_pid;
            //获取当前子进程pid
            ce_pid = getpid();
            //进入子进程循环
            return pid;

        default:
            break;
    }

    php_printf("start %s %P", name, pid);

    //fork 返回成功后  是子进程的pid
    processes[s].pid = pid;
    processes[s].exited = 0;

    if (respawn >= 0) {
        return pid;
    }

    processes[s].proc = proc;
    processes[s].data = data;
    processes[s].name = name;
    processes[s].exiting = 0;

    switch (respawn) {

        case PROCESS_NORESPAWN:
            processes[s].respawn = 0;
            processes[s].just_spawn = 0;
            processes[s].detached = 0;
            break;

        case PROCESS_JUST_SPAWN:
            processes[s].respawn = 0;
            processes[s].just_spawn = 1;
            processes[s].detached = 0;
            break;

        case PROCESS_RESPAWN:
            processes[s].respawn = 1;
            processes[s].just_spawn = 0;
            processes[s].detached = 0;
            break;

        case PROCESS_JUST_RESPAWN:
            processes[s].respawn = 1;
            processes[s].just_spawn = 1;
            processes[s].detached = 0;
            break;

        case PROCESS_DETACHED:
            processes[s].respawn = 0;
            processes[s].just_spawn = 0;
            processes[s].detached = 1;
            break;
    }

    if (s == last_process) {
        last_process++;
    }

    return pid;
}


