#include <stdio.h>
#include <shell.h>

#include "xtimer.h"

extern int udp_send(int argc, char **argv);
extern int udp_server(int argc, char **argv);

const shell_command_t shell_commands[] = {
    {"udp_send", "send data over UDP and listen on UDP ports", udp_send},
    {"udp_server", "Start UDP server", udp_server},
    {NULL, NULL, NULL}
};

int main(void)
{
    //Start shell
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);


    return 0;

}
