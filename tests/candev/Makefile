APPLICATION = candev
include ../Makefile.tests_common

USEMODULE += shell
USEMODULE += can
USEMODULE += isrpipe

# define the CAN driver you want to use here
CAN_DRIVER ?= MCP2515

ifeq ($(CAN_DRIVER), MCP2515)
    FEATURES_REQUIRED = periph_gpio periph_spi periph_gpio_irq

    USEMODULE += mcp2515

	#set device paramters
	TEST_MCP2515_SPI   ?= SPI_DEV\(0\)
	TEST_MCP2515_SPI_CLK   ?= SPI_CLK_1MHZ
	TEST_MCP2515_SPI_MODE   ?= SPI_MODE_0
	TEST_MCP2515_CS    ?= GPIO_PIN\(PORT_B,4\)
	TEST_MCP2515_RESET ?= GPIO_PIN\(PORT_B,2\)
	TEST_MCP2515_INT ?= GPIO_PIN\(PORT_B,3\)
	TEST_MCP2515_CLK ?=\(8000000ul\)
	TEST_MCP2515_BITRATE ?= \(125000\)

	#INT_PIN = PB3 --> requires AVR bank1 on avr. Change accordingly to used hardware.
	USEMODULE += atmega_pcint1

	# export parameters
	CFLAGS += -DTEST_MCP2515_SPI=$(TEST_MCP2515_SPI)
	CFLAGS += -DTEST_MCP2515_SPI_MODE=$(TEST_MCP2515_SPI_MODE)
	CFLAGS += -DTEST_MCP2515_SPI_CLK=$(TEST_MCP2515_SPI_CLK)
	CFLAGS += -DTEST_MCP2515_CS=$(TEST_MCP2515_CS)
	CFLAGS += -DTEST_MCP2515_RESET=$(TEST_MCP2515_RESET)
	CFLAGS += -DTEST_MCP2515_INT=$(TEST_MCP2515_INT)
	CFLAGS += -DTEST_MCP2515_CLK=$(TEST_MCP2515_CLK)
	CFLAGS += -DCANDEV_MCP2515_DEFAULT_BITRATE=$(TEST_MCP2515_BITRATE)

else ifeq ($(CAN_DRIVER), PERIPH_CAN)
# periph_can modules/variables go here

else ifeq ($(CAN_DRIVER), CAN_NATIVE)
# can_native modules/variables go here

endif

CFLAGS += -DCAN_DRIVER_$(CAN_DRIVER)

include $(RIOTBASE)/Makefile.include
