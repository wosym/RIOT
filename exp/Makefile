###
### Experiment to make a UDP application that will send a reply to udp packets it receives
###


# Set the name of your application:
APPLICATION = udp_ping_pong

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../../RIOT

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


BOARD_PROVIDES_NETIF := nucleo-f207zg

# Modules to include:

USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps
#USEMODULE += posix_headers
#USEMODULE += xtimer
#USEMODULE += gnrc_pktdump
USEMODULE += gnrc_netdev_default
USEMODULE += auto_init_gnrc_netif
#USEMODULE += gnrc_txtsnd
USEMODULE += gnrc_ipv6_default
USEMODULE += gnrc_icmpv6_echo
USEMODULE += gnrc_sock_udp

# If your application is very simple and doesn't use modules that use
# messaging, it can be disabled to save some memory:

#DISABLE_MODULE += core_msg

#INCLUDES += -Iapplication_include

# Specify custom dependencies for your application here ...
# APPDEPS = app_data.h config.h

include $(RIOTBASE)/Makefile.include

# ... and define them here (after including Makefile.include,
# otherwise you modify the standard target):
#proj_data.h: script.py data.tar.gz
#	./script.py

include $(RIOTMAKE)/default-radio-settings.inc.mk
