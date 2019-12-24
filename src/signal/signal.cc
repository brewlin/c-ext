//
// Created by Administrator on 2019/12/23 0023.
//
///#include "signal.h"
#include "php_lib.h"

//向linux 注册信号，和回调监听方法
int_t  init_signal(signal_t sig)
{
    struct sigaction   sa;

    memzero(&sa, sizeof(struct sigaction));

    if (sig->handler) {
        sa.sa_sigaction = sig->handler;
        sa.sa_flags = SA_SIGINFO;

    } else {
        sa.sa_handler = SIG_IGN;
    }

    sigemptyset(&sa.sa_mask);
        if (sigaction(sig->signo, &sa, NULL) == -1) {
#if (VALGRIND)
            php_ptrintf(
                          "sigaction(%s) failed, ignored", sig->signame);
#else
            php_ptrintf(
                          "sigaction(%s) failed", sig->signame);
            return ERROR;
#endif
        }

    return OK;
}


static void signal_handler(int signo, siginfo_t *siginfo, void *ucontext)
{
    char            *action;
    int_t        ignore;
    err_t        err;
    signal_t    *sig;

    ignore = 0;

    err = errno;

    for (sig = signals; sig->signo != 0; sig++) {
        if (sig->signo == signo) {
            break;
        }
    }

    time_sigsafe_update();

    action = "";

    switch (process) {

        case PROCESS_MASTER:
        case PROCESS_SINGLE:
            switch (signo) {

                case signal_value(SHUTDOWN_SIGNAL):
                    quit = 1;
                    action = ", shutting down";
                    break;

                case signal_value(TERMINATE_SIGNAL):
                case SIGINT:
                    terminate = 1;
                    action = ", exiting";
                    break;

                case signal_value(NOACCEPT_SIGNAL):
                    if (daemonized) {
                        noaccept = 1;
                        action = ", stop accepting connections";
                    }
                    break;

                case signal_value(RECONFIGURE_SIGNAL):
                    reconfigure = 1;
                    action = ", reconfiguring";
                    break;

                case signal_value(REOPEN_SIGNAL):
                    reopen = 1;
                    action = ", reopening logs";
                    break;

                case signal_value(CHANGEBIN_SIGNAL):
                    if (getppid() == parent || new_binary > 0) {

                        /*
                         * Ignore the signal in the new binary if its parent is
                         * not changed, i.e. the old binary's process is still
                         * running.  Or ignore the signal in the old binary's
                         * process if the new binary's process is already running.
                         */

                        action = ", ignoring";
                        ignore = 1;
                        break;
                    }

                    change_binary = 1;
                    action = ", changing binary";
                    break;

                case SIGALRM:
                    sigalrm = 1;
                    break;

                case SIGIO:
                    sigio = 1;
                    break;

                case SIGCHLD:
                    reap = 1;
                    break;
            }

            break;

        case PROCESS_WORKER:
        case PROCESS_HELPER:
            switch (signo) {

                case signal_value(NOACCEPT_SIGNAL):
                    if (!daemonized) {
                        break;
                    }
                    debug_quit = 1;
                    /* fall through */
                case signal_value(SHUTDOWN_SIGNAL):
                    quit = 1;
                    action = ", shutting down";
                    break;

                case signal_value(TERMINATE_SIGNAL):
                case SIGINT:
                    terminate = 1;
                    action = ", exiting";
                    break;

                case signal_value(REOPEN_SIGNAL):
                    reopen = 1;
                    action = ", reopening logs";
                    break;

                case signal_value(RECONFIGURE_SIGNAL):
                case signal_value(CHANGEBIN_SIGNAL):
                case SIGIO:
                    action = ", ignoring";
                    break;
            }

            break;
    }

    if (siginfo && siginfo->si_pid) {
        php_ptrintf(
                      "signal %d (%s) received from %P%s",
                      signo, sig->signame, siginfo->si_pid, action);

    } else {
        php_ptrintf(
                      "signal %d (%s) received%s",
                      signo, sig->signame, action);
    }

    if (ignore) {
        log_error(LOG_CRIT, cycle->log, 0,
                      "the changing binary signal is ignored: "
                      "you should shutdown or terminate "
                      "before either old or new binary's process");
    }

    if (signo == SIGCHLD) {
        process_get_status();
    }

    set_errno(err);
}