APPLICATION = cv_hmi

BOARD ?= atmega328p
PROGRAMMER ?= usbtiny

RIOTBASE ?= $(CURDIR)/../..

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

QUIET ?= 1

USEMODULE += periph_adc
USEMODULE += periph_gpio
USEMODULE += xtimer
USEMODULE += shell
USEMODULE += can
USEMODULE += isrpipe
USEMODULE += mcp2515

FEATURES_REQUIRED = periph_gpio periph_spi periph_gpio_irq

#set device paramters
TEST_MCP2515_SPI   ?= SPI_DEV\(0\)
TEST_MCP2515_SPI_CLK   ?= SPI_CLK_1MHZ
TEST_MCP2515_SPI_MODE   ?= SPI_MODE_0
TEST_MCP2515_CS    ?= GPIO_PIN\(PORT_B,0\)
TEST_MCP2515_RESET ?= GPIO_PIN\(PORT_B,2\)
TEST_MCP2515_INT ?= GPIO_PIN\(PORT_B,1\)
TEST_MCP2515_CLK ?=\(8000000ul\)
TEST_MCP2515_BITRATE ?= \(125000\)

USEMODULE += atmega_pcint2	#PORT_D --> encoder
USEMODULE += atmega_pcint1
USEMODULE += atmega_pcint0

# export parameters
CFLAGS += -DTEST_MCP2515_SPI=$(TEST_MCP2515_SPI)
CFLAGS += -DTEST_MCP2515_SPI_MODE=$(TEST_MCP2515_SPI_MODE)
CFLAGS += -DTEST_MCP2515_SPI_CLK=$(TEST_MCP2515_SPI_CLK)
CFLAGS += -DTEST_MCP2515_CS=$(TEST_MCP2515_CS)
CFLAGS += -DTEST_MCP2515_RESET=$(TEST_MCP2515_RESET)
CFLAGS += -DTEST_MCP2515_INT=$(TEST_MCP2515_INT)
CFLAGS += -DTEST_MCP2515_CLK=$(TEST_MCP2515_CLK)
CFLAGS += -DCANDEV_MCP2515_DEFAULT_BITRATE=$(TEST_MCP2515_BITRATE)

LINKFLAGS += -Wl,-u,vfprintf -lprintf_flt -lm


include $(RIOTBASE)/Makefile.include
