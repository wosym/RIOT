#ifndef INCLUDED_GLCD_IO_AVRGCC_ATTINY2313_H
#define INCLUDED_GLCD_IO_AVRGCC_ATTINY2313_H

/*
    This file defines the pin mapping between the GLCD and the ATtiny2313
    using AVR-GCC and AVR Libc. This file should be included before glcd.h. 
    This file also defines some macro's for dealing with data to be stored
    in flash memory.

    Copyright 2013 Peter van Merkerk

    This file is part of the GLCD SED1531 library.

    The GLCD SED1531 library is free software: you can redistribute it and/or
    modify it under the terms of the GNU General Public License as published
    by the Free Software Foundation, either version 3 of the License, or (at
    your option) any later version.

    The GLCD SED1531 library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
    Public License for more details.

    You should have received a copy of the GNU General Public License along with
    the GLCD SED1531 library If not, see http://www.gnu.org/licenses/.
*/

/*
    Port and pin mapping for ATtiny2313 @ 1MHz:

    AVR      GLCD
    -----    ----
    PB0-PB7  D0-D7
    PD0      A0
    PD1      RW
    PD2      E 
*/
#include <avr/io.h>

#define GLCD_IO_INIT()                  DDRD = 0x07;

#define GLCD_IO_PIN_A0_0()              PORTD &= ~_BV(PD0)
#define GLCD_IO_PIN_A0_1()              PORTD |= _BV(PD0)

#define GLCD_IO_PIN_RW_0()              PORTD &= ~_BV(PD1)
#define GLCD_IO_PIN_RW_1()              PORTD |= _BV(PD1)

#define GLCD_IO_PIN_E_0()               PORTD &= ~_BV(PD2)
#define GLCD_IO_PIN_E_1()               PORTD |= _BV(PD2)

#define GLCD_IO_DATA_OUTPUT(data)       PORTB = (data)
#define GLCD_IO_DATA_INPUT()            PINB

#define GLCD_IO_DATA_DIR_INPUT()        DDRB = 0x00;
#define GLCD_IO_DATA_DIR_OUTPUT()       DDRB = 0xFF;

#define GLCD_IO_DELAY_READ()            __asm volatile ("nop")

/* AVR-GCC Flash definitions */
#include <avr/pgmspace.h>

#define GLCD_FLASH(type, name)          const type const name PROGMEM
#define GLCD_FLASH_READ_BYTE(address)   pgm_read_byte(address)         
#define GLCD_FLASH_READ_WORD(address)   pgm_read_word(address) 
#define GLCD_FLASH_PTR(type)            const type* PROGMEM

#endif // #ifndef INCLUDED_IO_GLCD_AVRGCC_ATTINY2313_H
