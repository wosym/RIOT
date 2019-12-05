#include "netutil.h"

//code moved from lwip_bootstrap to here, to make it work. TODO: needs proper solution!
int start_dhcp(void)
{
    static netdev_t stm32eth;
    extern void stm32_eth_netdev_setup(netdev_t *netdev);
    static struct netif netif[1];

    stm32_eth_netdev_setup(&stm32eth);
    int ret = 0;

    if (netif_add(&netif[0], IP4_ADDR_ANY, IP4_ADDR_ANY, IP4_ADDR_ANY,
                  &stm32eth, lwip_netdev_init, tcpip_input) == NULL) {
        puts("Could not add stm32_eth device\n");
        return -1;
    }
    ret = dhcp_start(&netif[0]);
    if (ret) {
        printf("DHCP_start ret: %d\n", ret);
        return -1;
    }
    sys_check_timeouts();
    ret = dhcp_supplied_address(&netif[0]);
    if (ret) {
        printf("DHCP_supplied_address ret: %d\n", ret);
        return -1;
    }
    return 0;
}

int ifconfig(int argc, char **argv)
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
                printf(" inet6 %s\n",
                       ipv6_addr_to_str(addrstr,
                                        (ipv6_addr_t *)&iface->ip6_addr[i],
                                        sizeof(addrstr)));
            }
        }
#endif
#ifndef APP_IPV6
        printf("MAC: %u:%u:%u:%u:%u:%u\n", iface->hwaddr[0], iface->hwaddr[1],
               iface->hwaddr[2], iface->hwaddr[3], iface->hwaddr[4],
               iface->hwaddr[5]);

        printf("\tIP: %s\n", \
               ipaddr_ntoa(&iface->ip_addr));

#endif
        puts("");
    }
    return 0;
}
