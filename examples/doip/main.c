#include <ctype.h>
#include <doip.h>
#include <shell.h>
#include <stdio.h>
#include <string.h>
#include <xtimer.h>

#include "netutil.h"

#define T_2_SEC 2
#define T_1_SEC 1
#define SAMPLE_SA 0xe801
#define SAMPLE_TA 0x1fff
#define SAMPLE_IP "192.168.0.11"
#define IPSTRLEN 16

sock_doip_t sock;
char target_ip[IPSTRLEN] = SAMPLE_IP;

static int print_target_ip(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    printf("Current target IP: %s\n", target_ip);

    return 0;
}
static int set_target_ip(int argc, char **argv)
{
    int i = 0;
    uint8_t ip_len = 0; //len of ipstring
    uint8_t nd = 0;     //number of decimal points
    uint8_t pd = 1;     //previous character was a decimal point flag

    if(argc < 2) {
        puts("Usage: set_target_ip <ip>");
        return -1;
    }
    ip_len = strlen(argv[1]);
    if( ip_len > IPSTRLEN)
    {
        puts("Wrong IP format. Required format (a): xxx.xxx.xxx.xxx");
        return -1;
    }

    //Check if ip is of correct format  //TODO: seperate util-function?
    for(i = 0; i < ip_len; i++) {
        if(argv[1][i] == '.') {    //char is a .
            if(pd) {            //previous char was also a . --> not allowed
                puts("Wrong IP format (b). Required format: xxx.xxx.xxx.xxx");
                return -1;
            } else {            //previous char was not a .
                nd++;
                pd=1;
            }
        } else {                //char is not a .
            if(!isdigit((unsigned char)argv[1][i])) {
                puts("Wrong IP format (c). Required format: xxx.xxx.xxx.xxx");
                return -1;
            }
            pd = 0;
        }
    }
    if(pd) {                    //last char was a . --> not allowed
        puts("Wrong IP format (d). Required format: xxx.xxx.xxx.xxx");
        return -1;
    }

    strncpy(target_ip, argv[1], IPSTRLEN);
    printf("New target IP: %s\n", target_ip);

    return 0;
}
static int spam_doip(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    char ip_addr[] = SAMPLE_IP;
    doip_sa source = SAMPLE_SA;
    doip_ta target = SAMPLE_TA;
    uint8_t uds_data[] = { UDS_SERVICES_TP, UDS_SUPRESS_REPLY };

    while (1) {
        doip_send_udp(&sock, source, target, DOIP_DIAGNOSTIC_MESSAGE, uds_data, 2,
                      ip_addr);
        doip_send_udp(&sock, 0, 0, DOIP_VEHICLE_IDENTIFICATION_REQUEST, NULL, 0,
                      ip_addr);
        xtimer_sleep(T_1_SEC);
    }
    return 0;
}

static int send_doip(int argc, char **argv)
{
    doip_sa source = 0;
    doip_ta target = 0;
    uint8_t uds_data[] = { UDS_SERVICES_TP, UDS_SUPRESS_REPLY };
    int dlen = 0;
    uint16_t msg = 0;

    printf("n: %d\n", argc);
    if (argc < 2) {
        goto help;
    }
    if (argc < 3) {
        msg = DOIP_VEHICLE_IDENTIFICATION_REQUEST;
    }
    if (msg == DOIP_VEHICLE_IDENTIFICATION_REQUEST) {
        source = 0;
        target = 0;
        dlen = 0;
    }
    else {
        source = SAMPLE_SA;
        target = SAMPLE_TA;
        dlen = 2;
    }

    doip_send_udp(&sock, source, target, msg, uds_data, dlen, argv[1]);



    return 0;

help:
    puts("Usage: send_doip <ip> [<doip_message_type> <n> <delay>]");
    return -1;
}

const shell_command_t shell_commands[] = {
    { "ifconfig", "Show network interfaces", ifconfig },
    { "set_target_ip", "Set IP address to which the DoIP messages should be sent", set_target_ip },
    { "print_target_ip", "Print current IP address to which the DoIP messages should be sent", print_target_ip },
    { "spam_doip", "Continiously send doip requests", spam_doip },
    { "send_doip", "Send doip message", send_doip },
    { NULL, NULL, NULL }
};

int main(void)
{
    int ret = 0;
    char line_buf[SHELL_DEFAULT_BUFSIZE];

    puts("Starting doip example");
    xtimer_sleep(T_2_SEC);         //necesarry for some reason when sending UDP
    ret = start_dhcp();
    if (ret) {
        puts("failed to start dhcp service");
        while (1) {}
    }

    sock_doip_create(&sock);    //initialize DoIP socket

    xtimer_sleep(T_2_SEC);     //Give it some extra time to get the DHCP-offer

    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
