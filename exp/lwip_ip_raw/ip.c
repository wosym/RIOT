#include <string.h>
#include "util.h"
#include "net/ipv6.h"
#include "net/sock/ip.h"
#include "shell.h"
#include "xtimer.h"


int ip_send_packet(char *addr_str, int nxt, char *data, int dlen)
{
    int ret = 0;

#ifdef APP_IPV6
    sock_ip_ep_t dst = SOCK_IPV6_EP_ANY;

    /* parse destination address */
    if (ipv6_addr_from_str((ipv6_addr_t *)&dst.addr.ipv6, addr_str) == NULL) {
        puts("Error: unable to parse destination address");
        return 1;
    }
#else
    sock_ip_ep_t dst = SOCK_IPV4_EP_ANY;

    /* parse destination address */
    if (ipv4_addr_from_str((ipv4_addr_t *)&dst.addr.ipv4, addr_str) == NULL) {
        puts("Error: unable to parse destination address");
        return 1;
    }

#endif

    //xtimer_usleep(2000000);

    //sock_ip_t *sock = NULL;
    ret = sock_ip_send(NULL, data, dlen, nxt, &dst);
    if(ret < 0) {
        puts("could not send");
        printf("Error: %d  %x", ret, ret);
        puts("");
        return -1;
    } else {
        puts("Package sent");
        return 0;
    }


    return 0;


}
