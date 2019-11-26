#include <stdio.h>
#include <shell.h>
#include "lwip.h"
#include "lwip/netif.h"
#include "net/ipv6/addr.h"
#include "xtimer.h"
#include "ip.h"


//#define MODULE_STM32_ETH

static int ifconfig(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    puts("ifconfig IPV6");
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

const shell_command_t shell_commands[] = {
    {"ifconfig", "Show network interfaces", ifconfig},
    {NULL, NULL, NULL}
};

int main(void)
{
    int ret = 0;
    char* ip = "fe80::9333:621c:1f7c:96fd";
    //char* ip = "255.255.255.255";
    char prot = 41;
    char data[] = {50, 50, 50, 50};
    int dlen = 3;
    //Start shell
    char line_buf[SHELL_DEFAULT_BUFSIZE];

    //printf("---%s---\n", NETMASK);

    xtimer_usleep(2000000);

    ret = ip_send_packet(ip, prot, data, dlen);
    if(ret)
        puts("Error sending ip packet");
    else
        puts("ip packet sent");
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);


    return 0;

}
