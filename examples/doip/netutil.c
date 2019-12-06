#include "netutil.h"

int start_dhcp(void)
{
    int ret = 0;
    struct netif *netif = netif_find("ET");

    ret = dhcp_start(netif);
    if (ret) {
        printf("DHCP_start ret: %d\n", ret);
        return -1;
    }
    sys_check_timeouts();
    ret = dhcp_supplied_address(netif);
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
#else
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
