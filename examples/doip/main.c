#include <stdio.h>
#include <xtimer.h>
#include <doip.h>
#include <shell.h>
#include "netutil.h"

static int spam_doip(int argc, char** argv)
{
    (void) argc;
    (void) argv;
    char ip_addr[] = "192.168.0.20";
    doip_sa source = 0xe801;
    doip_ta target = 0x1fff;
    uint8_t uds_data[] = {UDS_SERVICES_TP, UDS_SUPRESS_REPLY};

    while(1) {
        doip_send_udp(source, target, DOIP_DIAGNOSTIC_MESSAGE, uds_data , 2, ip_addr);
        doip_send_udp(0, 0, DOIP_VEHICLE_IDENTIFICATION_REQUEST, NULL , 0, ip_addr);
        xtimer_usleep(1000000);
    }
    return 0;
}

static int send_doip(int argc, char** argv)
{
    doip_sa source = 0;
    doip_ta target = 0;
    uint8_t uds_data[] = {UDS_SERVICES_TP, UDS_SUPRESS_REPLY};
    int dlen = 0;
    uint16_t msg = 0;
    printf("n: %d\n", argc);
    if(argc < 2)
        goto help;
    if(argc < 3) {
        msg = DOIP_VEHICLE_IDENTIFICATION_REQUEST;
    }
    if(msg == DOIP_VEHICLE_IDENTIFICATION_REQUEST){
        source = 0;
        target = 0;
        dlen = 0;
    } else {
        source = 0xe801;
        target = 0x1fff;
        dlen = 2;
    }

    doip_send_udp(source, target, msg, uds_data, dlen, argv[1]);



    return 0;

help:
    puts("Usage: send_doip <ip> [<doip_message_type> <n> <delay>]");
    return -1;
}

const shell_command_t shell_commands[] = {
    {"ifconfig", "Show network interfaces", ifconfig},
    {"spam_doip", "Continiously send doip requests", spam_doip},
    {"send_doip", "Send doip message", send_doip},
    {NULL, NULL, NULL}
};

int main(void)
{
    int ret = 0;
    char line_buf[SHELL_DEFAULT_BUFSIZE];

    puts("Starting doip example");
    xtimer_usleep(2000000);     //necesarry for some reason when sending UDP
    ret = start_dhcp();
    if(ret){
        puts("failed to start dhcp service");
        while(1);
    }

    xtimer_usleep(2000000); //Give it some extra time to get the DHCP-offer

    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
