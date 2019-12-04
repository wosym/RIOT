# Set the name of your application:
APPLICATION = lwip_ip_test

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../../../RIOT

# Change this to 0 to show compiler invocation lines by default:
QUIET ?= 1

APP_IPV6 ?= 0

ifneq (1, $(APP_IPV6))
 IP_ADDR = \"192.168.1.21\"
 NETMASK = \"255.255.255.0\"
 GATEWAY = \"192.168.1.1\"
endif

BOARD := nucleo-f207zg
BOARD_PROVIDES_NETIF := nucleo-f207zg

DEBUG = 1

# Modules to include:

USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps
#USEMODULE += posix_headers
#USEMODULE += xtimer

ifneq (0, $(APP_IPV6))
 USEMODULE += ipv6_addr
 USEMODULE += lwip_ipv6_autoconfig
 CFLAGS += -DAPP_IPV6
else
 USEMODULE += ipv4_addr
 USEMODULE += lwip_arp
 USEMODULE += lwip_ipv4
 CFLAGS += -DETHARP_SUPPORT_STATIC_ENTRIES=1
 CFLAGS += -DIP_ADDR=$(IP_ADDR)
 CFLAGS += -DNETMASK=$(NETMASK)
 CFLAGS += -DGATEWAY=$(GATEWAY)
endif


USEMODULE += lwip_netdev
USEMODULE += lwip_sock_udp
USEMODULE += lwip lwip_sock_ip lwip_netdev
USEMODULE += lwip_udp lwip_sock_udp
USEMODULE += lwip_tcp lwip_sock_tcp
#USEMODULE += inet_csum
#USEMODULE += netdev_test

USEMODULE += netdev_default

USEMODULE += lwip_dhcp
CFLAGS += -DUSE_DHCP

#CFLAGS += -DMEM_USE_POOLS=1 -DMEMP_USE_CUSTOM_POOLS=1

include $(RIOTBASE)/Makefile.include
