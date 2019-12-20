//
// Created by Administrator on 2019/12/19 0019.
//
#include "channel.h"
#include "php_lib.h"




int_t write_channel(socket_t s, channel_t *ch, size_t size)
{
    size_t             n;
    struct iovec        iov[1];
    struct msghdr       msg;
    err_t           err;

#if (HAVE_MSGHDR_MSG_CONTROL)

    union {
        struct cmsghdr  cm;
        char            space[CMSG_SPACE(sizeof(int))];
    } cmsg;

    if (ch->fd == -1) {
        msg.msg_control = NULL;
        msg.msg_controllen = 0;

    } else {
        msg.msg_control = (caddr_t) &cmsg;
        msg.msg_controllen = sizeof(cmsg);

        memzero(&cmsg, sizeof(cmsg));

        cmsg.cm.cmsg_len = CMSG_LEN(sizeof(int));
        cmsg.cm.cmsg_level = SOL_SOCKET;
        cmsg.cm.cmsg_type = SCM_RIGHTS;

        /*
         * We have to use memcpy() instead of simple
         *   *(int *) CMSG_DATA(&cmsg.cm) = ch->fd;
         * because some gcc 4.4 with -O2/3/s optimization issues the warning:
         *   dereferencing type-punned pointer will break strict-aliasing rules
         *
         * Fortunately, gcc with -O1 compiles this memcpy()
         * in the same simple assignment as in the code above
         */

        memcpy(CMSG_DATA(&cmsg.cm), &ch->fd, sizeof(int));
    }

    msg.msg_flags = 0;

#else

    if (ch->fd == -1) {
//        msg.msg_accrights = NULL;
//        msg.msg_accrightslen = 0;

    } else {
//        msg.msg_accrights = (caddr_t) &ch->fd;
//        msg.msg_accrightslen = sizeof(int);
    }

#endif

    iov[0].iov_base = (char *) ch;
    iov[0].iov_len = size;

    msg.msg_name = NULL;
    msg.msg_namelen = 0;
    msg.msg_iov = iov;
    msg.msg_iovlen = 1;

    n = sendmsg(s, &msg, 0);

    if (n == -1) {
        err = errno;
        if (err == EAGAIN) {
            return AGAIN;
        }
        php_printf("sendmsg() failed　%d %s",err,strerror(err));
        return ERROR;
    }

    return OK;
}


int_t read_channel(socket_t s, channel_t *ch, size_t size)
{
    ssize_t             n;
    err_t           err;
    struct iovec        iov[1];
    struct msghdr       msg;

#if (HAVE_MSGHDR_MSG_CONTROL)
    union {
        struct cmsghdr  cm;
        char            space[CMSG_SPACE(sizeof(int))];
    } cmsg;
#else
    int                 fd;
#endif

    iov[0].iov_base = (char *) ch;
    iov[0].iov_len = size;

    msg.msg_name = NULL;
    msg.msg_namelen = 0;
    msg.msg_iov = iov;
    msg.msg_iovlen = 1;

#if (HAVE_MSGHDR_MSG_CONTROL)
    msg.msg_control = (caddr_t) &cmsg;
    msg.msg_controllen = sizeof(cmsg);
#else
//    msg.msg_accrights = (caddr_t) &fd;
//    msg.msg_accrightslen = sizeof(int);
#endif

    n = recvmsg(s, &msg, 0);

    if (n == -1) {
        err = errno;
        if (err == EAGAIN) {
            return AGAIN;
        }

        php_printf("recvmsg() failed");
        return ERROR;
    }

    if (n == 0) {
        php_printf("recvmsg() returned zero");
        return ERROR;
    }

    if ((size_t) n < sizeof(channel_t)) {
        php_printf( "recvmsg() returned not enough data: %z", n);
        return ERROR;
    }

#if (HAVE_MSGHDR_MSG_CONTROL)

    if (ch->command == CMD_OPEN_CHANNEL) {

        if (cmsg.cm.cmsg_len < (socklen_t) CMSG_LEN(sizeof(int))) {
            php_printf("recvmsg() returned too small ancillary data");
            return ERROR;
        }

        if (cmsg.cm.cmsg_level != SOL_SOCKET || cmsg.cm.cmsg_type != SCM_RIGHTS)
        {
            php_printf(
                          "recvmsg() returned invalid ancillary data "
                          "level %d or type %d",
                          cmsg.cm.cmsg_level, cmsg.cm.cmsg_type);
            return ERROR;
        }

        /* ch->fd = *(int *) CMSG_DATA(&cmsg.cm); */

        memcpy(&ch->fd, CMSG_DATA(&cmsg.cm), sizeof(int));
    }

    if (msg.msg_flags & (MSG_TRUNC|MSG_CTRUNC)) {
        php_printf(
                      "recvmsg() truncated data");
    }

#else

//    if (ch->command == CMD_OPEN_CHANNEL) {
//        if (msg.msg_accrightslen != sizeof(int)) {
//            php_printf( "recvmsg() returned no ancillary data");
//            return ERROR;
//        }

        ch->fd = fd;
//    }

#endif

    return OK;
}



//关闭通道
void close_channel(fd_t *fd)
{
    if (close(fd[0]) == -1) {
        php_printf("close() channel failed");
    }

    if (close(fd[1]) == -1) {
        php_printf("close() channel failed");
    }
}