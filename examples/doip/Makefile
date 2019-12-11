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

#IP_ADDR = \"169.254.73.149\"
#NETMASK = \"255.255.255.0\"
#GATEWAY = \"169.254.73.148\"
IP_ADDR = \"192.168.1.21\"
NETMASK = \"255.255.255.0\"
GATEWAY = \"192.168.1.255\"
USEMODULE += ipv4_addr
USEMODULE += lwip_arp
USEMODULE += lwip_dhcp
USEMODULE += lwip_ipv4
USEMODULE += lwip_netdev
USEMODULE += lwip_sock_udp
USEMODULE += lwip lwip_sock_ip lwip_netdev
USEMODULE += lwip_udp lwip_sock_udp
USEMODULE += lwip_tcp lwip_sock_tcp
USEMODULE += netdev_default
USEMODULE += xtimer
USEMODULE += shell

#CFLAGS += -DMEM_USE_POOLS=1 -DMEMP_USE_CUSTOM_POOLS=1

CFLAGS += -DETHARP_SUPPORT_STATIC_ENTRIES=1
CFLAGS += -DIP_ADDR=$(IP_ADDR)
CFLAGS += -DNETMASK=$(NETMASK)
CFLAGS += -DGATEWAY=$(GATEWAY)

CFLAGS += -DUSE_DHCP    #necesarry because we don't want the regular bootstrap code to run
CFLAGS += -DLWIP_SO_RCVTIMEO	#necesarry to enable timeouts for lwip

include $(RIOTBASE)/Makefile.include
