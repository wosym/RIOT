/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdio.h>
#include <xtimer.h>
#include <doip.h>
#include <shell.h>

#include <lwip/dhcp.h>
#include "lwip/netif/netdev.h"
#include "stm32_eth.h"

//code moved from lwip_bootstrap to here, to make it work. TODO: needs proper solution!
int start_dhcp(void)
{
    static netdev_t stm32eth;
    extern void stm32_eth_netdev_setup(netdev_t *netdev);
    static struct netif netif[1];
    stm32_eth_netdev_setup(&stm32eth);
    int ret = 0;

    if (netif_add(&netif[0], IP4_ADDR_ANY, IP4_ADDR_ANY, IP4_ADDR_ANY, &stm32eth, lwip_netdev_init, tcpip_input) == NULL) {
        puts("Could not add stm32_eth device\n");
        return -1;
    }
    xtimer_usleep(2000000);
    ret = dhcp_start(&netif[0]);
    if(ret) {
        printf("DHCP_start ret: %d\n", ret);
        return -1;
    }
    sys_check_timeouts();
    ret = dhcp_supplied_address(&netif[0]);
    if(ret) {
        printf("DHCP_supplied_address ret: %d\n", ret);
        return -1;
    }
    return 0;
}

static int ifconfig(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    puts("ifconfig");
    for (struct netif *iface = netif_list; iface != NULL; iface = iface->next) {
        puts("test");
        printf("%s_%02u: ", iface->name, iface->num);
#ifdef MODULE_LWIP_IPV6
        char addrstr[IPV6_ADDR_MAX_STR_LEN];
        for (int i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
            if (!ipv6_addr_is_unspecified((ipv6_addr_t *)&iface->ip6_addr[i])) {
                printf(" inet6 %s\n", ipv6_addr_to_str(addrstr, (ipv6_addr_t *)&iface->ip6_addr[i],
                                                       sizeof(addrstr)));
            }
        }
#endif
#ifndef APP_IPV6
        printf("MAC: %u:%u:%u:%u:%u:%u\n", iface->hwaddr[0], iface->hwaddr[1], iface->hwaddr[2], iface->hwaddr[3], iface->hwaddr[4], iface->hwaddr[5]);

        printf("\tIP: %s\n",\
                ipaddr_ntoa(&iface->ip_addr));

#endif
        puts("");
    }
    return 0;
}
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

const shell_command_t shell_commands[] = {
    {"ifconfig", "Show network interfaces", ifconfig},
    {"spam_doip", "Continiously send doip requests", spam_doip},
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
