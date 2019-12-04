#ifndef __NETUTIL_H__
#define __NETUTIL_H__
#include <lwip/dhcp.h>
#include "lwip/netif/netdev.h"
#include "stm32_eth.h"

/*
 * @brief Initialise network interface and dhcp
 *
 * @detailed This used to be in the lwip-bootstrap sequence, but was moved to a seperate function to be called by user, because it didn't work where it originally was.
 */
int start_dhcp(void);

/*
 * @brief shows network interfaces with their MAC adresses and IP's
 */
int ifconfig(int argc, char **argv);


#endif
