include ../Makefile.tests_common

USEMODULE += doip
USEMODULE += embunit

IP_ADDR = \"192.168.0.21\"
NETMASK = \"255.255.255.0\"
GATEWAY = \"192.168.0.1\"
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
