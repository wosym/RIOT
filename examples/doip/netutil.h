#ifndef __NETUTIL_H__
#define __NETUTIL_H__
#include <lwip/dhcp.h>
#include "lwip/netif/netdev.h"
#include "stm32_eth.h"

int start_dhcp(void);
int ifconfig(int argc, char **argv);


#endif
