# name of your application
APPLICATION = doip-example

# If no BOARD is found in the environment, use this default:
BOARD ?= nucleo-f207zg

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

USEMODULE += doip

IP_ADDR = \"169.254.73.149\"
NETMASK = \"255.255.255.0\"
GATEWAY = \"169.254.73.148\"
USEMODULE += ipv4_addr
USEMODULE += lwip_arp
USEMODULE += lwip_ipv4
CFLAGS += -DETHARP_SUPPORT_STATIC_ENTRIES=1
CFLAGS += -DIP_ADDR=$(IP_ADDR)
CFLAGS += -DNETMASK=$(NETMASK)
CFLAGS += -DGATEWAY=$(GATEWAY)
USEMODULE += lwip_netdev
USEMODULE += lwip_sock_udp
USEMODULE += lwip lwip_sock_ip lwip_netdev
USEMODULE += lwip_udp lwip_sock_udp
USEMODULE += lwip_tcp lwip_sock_tcp
USEMODULE += netdev_default
USEMODULE += xtimer

include $(RIOTBASE)/Makefile.include
