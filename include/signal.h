//
// Created by Administrator on 2019/12/23 0023.
//

#ifndef C_EXT_SIGNAL_H
#define C_EXT_SIGNAL_H

#include "lib.h"


#define SHUTDOWN_SIGNAL      QUIT
#define TERMINATE_SIGNAL     TERM
#define NOACCEPT_SIGNAL      WINCH
#define RECONFIGURE_SIGNAL   HUP


typedef struct {
    int     signo;
    char   *signame;
    char   *name;
    void  (*handler)(int signo, siginfo_t *siginfo, void *ucontext);
} signal_t;

//signal_t  signals[] = {
//        { signal_value(RECONFIGURE_SIGNAL),
//                "SIG" value(RECONFIGURE_SIGNAL),
//                "reload",
//                signal_handler
//        }
//};
//
////定义各种信号
//signal_t  signals[] = {
//        {
//                signal_value(RECONFIGURE_SIGNAL),
//                "SIG" value(RECONFIGURE_SIGNAL),
//                "reload",
//                signal_handler},
//
//        {       signal_value(NOACCEPT_SIGNAL),
//                "SIG" value(NOACCEPT_SIGNAL),
//                "",
//                signal_handler },
//
//        {
//                signal_value(TERMINATE_SIGNAL),
//                "SIG" value(TERMINATE_SIGNAL),
//                "stop",
//                signal_handler },
//
//        {
//                signal_value(SHUTDOWN_SIGNAL),
//                "SIG" value(SHUTDOWN_SIGNAL),
//                "quit",
//                signal_handler },
//
//        { SIGALRM, "SIGALRM", "", signal_handler },
//
//        { SIGINT, "SIGINT", "", signal_handler },
//
//        { SIGIO, "SIGIO", "", signal_handler },
//
//        { SIGCHLD, "SIGCHLD", "", signal_handler },
//
//        { SIGSYS, "SIGSYS, SIG_IGN", "", NULL },
//
//        { SIGPIPE, "SIGPIPE, SIG_IGN", "", NULL },
//
//        { 0, NULL, "", NULL }
//};
int_t  init_signal(signal_t sig);

#endif //C_EXT_SIGNAL_H
