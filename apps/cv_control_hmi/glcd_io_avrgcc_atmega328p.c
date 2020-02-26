#include "glcd_io_avrgcc_atmega328p.h"
#include "atmega_gpio.h"


void GLCD_IO_DATA_OUTPUT(unsigned char data)
{  
    PORTD = ((PORTD & 0b00000011) | (data & 0b11111100));
    PORTC = ((PORTC & 0b11111100) | ((data & 0b00000011)));
    return;
    for (int i = 2; i <= 7; i++) {
        gpio_write(GPIO_PIN(PORT_D, i), (data >> i) & 0x01);
    }
    for (int i = 0; i <= 1; i ++) {
        gpio_write(GPIO_PIN(PORT_C, (i)), (data >> i) & 0x01);
    }
}
 
uint8_t GLCD_IO_DATA_INPUT(void)
{
    return ((PIND & 0b11111100) | (PINC & 0b00000011)); 

    char data = 0;
    for (int i = 0; i <= 7; i++) {
        if(i <= 1) {
            data |= (!!gpio_read(GPIO_PIN(PORT_C, (i)))) << i;
        } else {
            data |= (!!gpio_read(GPIO_PIN(PORT_D, i))) << i;
        }
    }

    return data;
}

void GLCD_IO_DATA_DIR_INPUT(void)
{  
    DDRD &= ~(0b11111100); 
    DDRC &= ~(0b00000011);

    return;
    gpio_init(GPIO_PIN(PORT_D, 2), GPIO_IN);
    gpio_init(GPIO_PIN(PORT_D, 3), GPIO_IN);
    gpio_init(GPIO_PIN(PORT_D, 4), GPIO_IN);
    gpio_init(GPIO_PIN(PORT_D, 5), GPIO_IN);
    gpio_init(GPIO_PIN(PORT_D, 6), GPIO_IN);
    gpio_init(GPIO_PIN(PORT_D, 7), GPIO_IN);

    gpio_init(GPIO_PIN(PORT_B, 3), GPIO_IN);
    gpio_init(GPIO_PIN(PORT_B, 4), GPIO_IN);
}

void GLCD_IO_DATA_DIR_OUTPUT(void)
{  
    DDRD |= 0b11111100;
    DDRC |= 0b00000011;

    return;
    gpio_init(GPIO_PIN(PORT_D, 2), GPIO_OUT);
    gpio_init(GPIO_PIN(PORT_D, 3), GPIO_OUT);
    gpio_init(GPIO_PIN(PORT_D, 4), GPIO_OUT);
    gpio_init(GPIO_PIN(PORT_D, 5), GPIO_OUT);
    gpio_init(GPIO_PIN(PORT_D, 6), GPIO_OUT);
    gpio_init(GPIO_PIN(PORT_D, 7), GPIO_OUT);
    gpio_init(GPIO_PIN(PORT_B, 3), GPIO_OUT);
    gpio_init(GPIO_PIN(PORT_B, 4), GPIO_OUT);
}

void GLCD_IO_INIT(void)
{
    gpio_init(GPIO_PIN(PORT_C, 2), GPIO_OUT);
    gpio_init(GPIO_PIN(PORT_C, 3), GPIO_OUT);
    gpio_init(GPIO_PIN(PORT_C, 4), GPIO_OUT);

}

