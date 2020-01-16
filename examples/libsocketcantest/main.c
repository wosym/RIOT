//#include <libsocketcan.h>
#include <stdio.h>
#include <thread.h>

#include "libsocketcan.h"

//#define WEIRD_FIX

#define THREAD_STACKSIZE   (THREAD_STACKSIZE_MAIN)
#define RECEIVE_THREAD_MSG_QUEUE_SIZE   (8)
#define RCV_THREAD_NUMOF (2)

static kernel_pid_t receive_pid[RCV_THREAD_NUMOF];

static void *tmpthr(void *args) {
    (void) args;
    return 0;
}

int main(int argc, char** argv)
{
    (void) argc;
    (void) argv;
    char iface[] = "vcan0";
    int res = 0;

    (void) res;    printf("_set: setting %s up\n", iface);
    res = can_do_start(iface);
    printf("res: %d\n", res);
    if (res < 0) {
        return res;
    }
    static char thread_stack[RCV_THREAD_NUMOF][THREAD_STACKSIZE];
    for (int i = 0; i < 2; i++) {
#ifdef WEIRD_FIX
        receive_pid[i] = thread_create(thread_stack[i], THREAD_STACKSIZE,
                                       THREAD_PRIORITY_MAIN - 1,
                                       THREAD_CREATE_STACKTEST, tmpthr,
                                       (void*)i, "receive_thread");
#endif
        (void) tmpthr;
        (void) thread_stack;
        (void) receive_pid;
    }

    puts("testing libsocketcan...");

    puts("getting state...");
    int state = 0;
    res = can_get_state(iface, &state);

    printf("state: %d res: %d\n", state, res);
    printf("_set: setting %s up\n", iface);
    res = can_do_start(iface);
    printf("res: %d\n", res);
    if (res < 0) {
        return res;
    }

    printf("_set: setting %s down\n", iface);

    res = can_do_stop(iface);
    printf("res: %d\n", res);
    if (res < 0) {
        return res;
    }

    printf("_set: setting %s up\n", iface);
    res = can_do_start(iface);
    printf("res: %d\n", res);
    if (res < 0) {
        return res;
    }


    return 0;
}

