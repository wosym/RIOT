###
###
###


# Set the name of your application:
APPLICATION = lwip_ip_test

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../../../RIOT

# Uncomment this to enable scheduler statistics for ps:
#CFLAGS += -DSCHEDSTATISTICS

# If you want to use native with valgrind, you should recompile native
# with the target all-valgrind instead of all:
# make -B clean all-valgrind

# Uncomment this to enable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
#DEVELHELP = 1

# Change this to 0 to show compiler invocation lines by default:
QUIET ?= 1

APP_IPV6 ?= 0

BOARD := nucleo-f207zg
BOARD_PROVIDES_NETIF := nucleo-f207zg

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
endif


USEMODULE += lwip_netdev
USEMODULE += lwip_sock_udp
USEMODULE += lwip lwip_sock_ip lwip_netdev
USEMODULE += lwip_udp lwip_sock_udp
USEMODULE += lwip_tcp lwip_sock_tcp
#USEMODULE += inet_csum
#USEMODULE += netdev_test

USEMODULE += netdev_default

include $(RIOTBASE)/Makefile.include
