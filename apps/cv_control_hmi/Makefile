APPLICATION = cv_hmi

BOARD ?= atmega328p
PROGRAMMER ?= usbtiny

RIOTBASE ?= $(CURDIR)/../..

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

QUIET ?= 1

USEMODULE += periph_gpio
USEMODULE += xtimer

#CFLAGS += -I$(CURDIR)/GLCD_SED1531/	#Why does this trigger missing-include dirs??

#USEMODULE += GLCD_SED1531
#EXTERNAL_MODULE_DIRS += $(CURDIR)/GLCD_SED1531
#INCDIRS += $(CURDIR)/GLCD_SED1531

include $(RIOTBASE)/Makefile.include
