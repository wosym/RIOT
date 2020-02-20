#include "glcd_io_avrgcc_atmega328p.h"

void GLCD_IO_DATA_OUTPUT(unsigned char data)
{  
    PORTD = ((PORTD & 0b00000011) | (data & 0b11111100));
    PORTB = ((PORTB & 0b11100111) | ((data & 0b00000011) << 3));
}
 
uint8_t GLCD_IO_DATA_INPUT()
{
    return ((PIND & 0b11111100) | (PINB & 0b00011000)); 
}

void GLCD_IO_DATA_DIR_INPUT()
{  
    DDRD &= ~(0b11111100); 
    DDRB &= ~(0b00011000);
}

void GLCD_IO_DATA_DIR_OUTPUT()
{  
    DDRD |= 0b11111100;
    DDRB |= 0b00011000;
}
