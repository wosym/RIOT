#include "glcd_io_avrgcc_atmega328p.h"
#include "atmega_gpio.h"
#include <xtimer.h>

#define SER     GPIO_PIN(PORT_D, 2)     //Serial in
#define RCLK    GPIO_PIN(PORT_D, 3)     //Latch
#define SRCLK   GPIO_PIN(PORT_D, 4)     //Clock
#define SR_DELAY    1

void clear_sr(void)
{
    for(int i = 0; i < 8; i++) {
        gpio_clear(SRCLK);
        gpio_clear(SER);
        xtimer_usleep(SR_DELAY);
        gpio_set(SRCLK);
        xtimer_usleep(SR_DELAY);
        gpio_clear(SRCLK);
        xtimer_usleep(SR_DELAY);


    }
    gpio_set(RCLK);
    xtimer_usleep(SR_DELAY);
    gpio_clear(RCLK);
}

void init_sr(void)
{
    gpio_init(SER, GPIO_OUT);
    gpio_init(RCLK, GPIO_OUT);
    gpio_init(SRCLK, GPIO_OUT);
    gpio_clear(SER);
    gpio_clear(RCLK);
    gpio_clear(SRCLK);

    clear_sr();


}

void shift_data(uint8_t data)
{
    (void) data;
    for(int i = 0; i < 8; i++) {
        gpio_clear(SRCLK);
        gpio_write(SER, (data >> (7- i)) & 0x01);
        xtimer_usleep(SR_DELAY);
        gpio_set(SRCLK);
        xtimer_usleep(SR_DELAY);
        gpio_clear(SRCLK);
        xtimer_usleep(SR_DELAY);


    }
    gpio_set(RCLK);
    xtimer_usleep(SR_DELAY);
    gpio_clear(RCLK);


}

void GLCD_IO_DATA_OUTPUT(unsigned char data)
{  
    shift_data(data);
    return;

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
    return 0;
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
    return;
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
    return;
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

