#ifndef INCLUDED_GLCD_IO_AVRGCC_ATMEGA328P_H
#define INCLUDED_GLCD_IO_AVRGCC_ATMEGA328P_H

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

//Probeersel om ze te mappen voor de atmega328p     --> Oké, Kan eigenlijk hetzelfde blijven, nee? Al deze poorten zijn ook beschikbaar op de m328p...
// --> TOCH NIET! We gebruiken een externe klok, dus kunnen we (denk ik toch) de pinnen PB6 en PB7 niet gebruiken! (want hier hangt het kristal aan) --> effe andere poorten gebruikt (B en D omgewisseld)
//Update: ik zou graag  PD0 en PD1 vrijhouden, zodat ze gebruikt kunnen worden voor U(S)ART. Proberen op te splitsen?

/*
    AVR      GLCD
    -----    ----
    PC0-PC1  D0-D1       
    PD2-PD7  D2-D7
    PB0      A0
    PB1      RW
    PB2      E 

*/

//Oké, volgens mij is er iets nie just me portC --> portB proberen? --> Zien da we nie in de miserie geraken me onze controle signalen
/*
    AVR      GLCD
    -----    ----
    PB3-PB4  D0-D1       
    PD2-PD7  D2-D7
    PB0      A0
    PB1      RW
    PB2      E 

*/

// Free PB pins for SPI!
/*
    AVR      GLCD
    -----    ----
    PC0-PC1  D0-D1       
    PD2-PD7  D2-D7
    PC2      A0
    PC3      RW
    PC4      E 

*/


#include <avr/io.h>

//#define GLCD_IO_INIT()                  DDRB = 0x07;

//#define GLCD_IO_PIN_A0_0()              PORTB &= ~_BV(PB0)
#define GLCD_IO_PIN_A0_0()              gpio_clear(GPIO_PIN(PORT_C, 2))
//#define GLCD_IO_PIN_A0_1()              PORTB |= _BV(PB0)
#define GLCD_IO_PIN_A0_1()              gpio_set(GPIO_PIN(PORT_C, 2))

//#define GLCD_IO_PIN_RW_0()              PORTB &= ~_BV(PB1)
#define GLCD_IO_PIN_RW_0()              gpio_clear(GPIO_PIN(PORT_C, 3))
//#define GLCD_IO_PIN_RW_1()              PORTB |= _BV(PB1)
#define GLCD_IO_PIN_RW_1()              gpio_set(GPIO_PIN(PORT_C, 3))

//#define GLCD_IO_PIN_E_0()               PORTB &= ~_BV(PB2)
#define GLCD_IO_PIN_E_0()               gpio_clear(GPIO_PIN(PORT_C, 4))
//#define GLCD_IO_PIN_E_1()               PORTB |= _BV(PB2)
#define GLCD_IO_PIN_E_1()               gpio_set(GPIO_PIN(PORT_C, 4))

//#define GLCD_IO_DELAY_READ()            __asm volatile ("nop")
#define GLCD_IO_DELAY_READ()            

/* AVR-GCC Flash definitions */
#include <avr/pgmspace.h>

#define GLCD_FLASH(type, name)          const type const name PROGMEM //NOTE: I removed PROGMEM HERE! Should we add PSTR at the call? How?
#define GLCD_FLASH_READ_BYTE(address)   pgm_read_byte(address)         
#define GLCD_FLASH_READ_WORD(address)   pgm_read_word(address) 
#define GLCD_FLASH_PTR(type)            const type* PROGMEM

void GLCD_IO_DATA_OUTPUT(unsigned char data);
 
uint8_t GLCD_IO_DATA_INPUT(void);

void GLCD_IO_DATA_DIR_INPUT(void);

void GLCD_IO_DATA_DIR_OUTPUT(void);

void GLCD_IO_INIT(void);

/*
void GLCD_IO_PIN_A0_0(void);
void GLCD_IO_PIN_A0_1(void);
void GLCD_IO_PIN_RW_0(void);
void GLCD_IO_PIN_RW_1(void);
void GLCD_IO_PIN_E_0(void);
void GLCD_IO_PIN_E_1(void);
*/

#endif // #ifndef INCLUDED_IO_GLCD_AVRGCC_ATMEGA328P_H


