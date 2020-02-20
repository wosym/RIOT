/*
    This is a small demonstration of the features of the GLCD SED1531 library.
    It currently targets 8-bit AVR's and is to be compiled with AVR-GCC 
    (or its windows counter part WinAVR), but it should be relatively easy to
    target other platforms and/or compilers.

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

/* Some unavoidable target and/or compiler specific stuff. */

/* Sets up the default speed for delay loops */
#define F_CPU 16000000UL	

#if defined(__GNUC__) && defined (__AVR_ATtiny2313__)

    #include <util/delay.h>    
    #define DELAY_MS(milliseconds) _delay_ms(milliseconds);     

    #include "glcd_io_avrgcc_attiny2313.h"

    /* Adding the noreturn attribute to main() reduces code size */ 
    int main(void) __attribute__((noreturn));

#elif defined(__IAR_SYSTEMS_ICC__)

  #include <iotiny2313.h>
  #include <pgmspace.h>
  #include "glcd_io_iar_2313.h"
  #define DELAY_MS(milliseconds) __delay_cycles((F_CPU/1000) * milliseconds)

#elif defined(M328P)
    #include "glcd_io_avrgcc_atmega328p.h"
    #include <avr/io.h>
    #include <util/delay.h>    
    #define DELAY_MS(milliseconds) _delay_ms(milliseconds);     



#else

    #error "Unsupported target and/or compiler."

#endif

#include "glcd.h"
#include "proportional_font.h"

static void effect_delay()
{
    DELAY_MS(20);
}

static void slide_delay()
{
    glcd_set_indicator(GLCD_INDICATOR_0,0); /* Adding this line somehow decreases code size (!?) */
    DELAY_MS(2000);
}

static unsigned char random()
{
    static unsigned char number = 0;

    number <<= 1;
    if((number & 0x80) == 0)
    {
        number ^= 0x4B;
    }

    return number;
}

static void slide_transition(glcd_flash_text_ptr title)
{
    unsigned char i;
    unsigned char left;
    unsigned char top;
    unsigned char width;
    unsigned char height;
    
    for(i = 0; i < 20; ++i)
    { 
        glcd_draw_box(0, 47-i, 100, 1, 1);
        glcd_draw_box(0, 8+i, 100, 1, 1);
        effect_delay();
    }

    glcd_draw_box(0, 0, 100, 8, 0);
    glcd_draw_text_P(0, 0, &proportional_font, title);

    for(i = 1; i < 20; ++i)
    { 
        left = 19 - i;
        top = 28 - i;
        width = i * 2 + 62;
        height = i * 2;

        glcd_draw_box(left, top, width, height, 0);
        effect_delay();
    }
}

static void slide_intro()
{
    unsigned char left = 0;
    unsigned char top = 4;
    unsigned char width = 97;
    unsigned char height = 40;
    static GLCD_FLASH_STRING(title, "GLCD Lib Demo on:");
    static GLCD_FLASH_STRING(line1, "- Atmel ATtiny2313");
    static GLCD_FLASH_STRING(line2, "- 8-bits, 1 MHz");
    static GLCD_FLASH_STRING(line3, "- 2KB Flash, 128B RAM");

    glcd_clear_display();

    glcd_draw_box(left+2,top+2, width, height, 1);
    glcd_draw_box(left,top,width, height, 0);
    glcd_draw_frame(left,top,width, height, 1, 1);

    glcd_draw_text_P(1, 5, &proportional_font, title);
    glcd_draw_text_P(2, 5, &proportional_font, line1);
    glcd_draw_text_P(3, 5, &proportional_font, line2);
    glcd_draw_text_P(4, 5, &proportional_font, line3);
}

static void slide_pixel()
{
    unsigned char x;
    unsigned char y;
    unsigned char d;
    static GLCD_FLASH_STRING(title, "GLCD Lib - Pixel");

    slide_transition(title);

    /* Draw background grid */
    glcd_draw_frame(0, 10, 100, 36, 1, 1);

    for(y = 16; y < 46; y += 8)
    {
        for(x = 2; x < 99; x += 2)
        {
           glcd_draw_pixel(x, y, 1); 
        }
    }

    /* Draw a semi-random graph on top of the grid. */
    y = 26;

    for(x = 1; x < 99; ++x)
    {
        glcd_draw_pixel(x, y, 1);
        d = random();

        if((d & 0x80) && (y < 45))
        {
           ++y;
        }
        else if(y > 12)
        {
           --y;
        }

        glcd_draw_pixel(x, y, 1);
    }
}

static void slide_line()
{
    unsigned char i;
    static GLCD_FLASH_STRING(title, "GLCD Lib - Line");

    slide_transition(title);

    for(i = 0; i < 100; i += 4)
    {
        glcd_draw_line(i, 10, 99-i, 45, 1);
    } 
}

static void slide_box()
{
    unsigned char x;
    unsigned char i;
    static GLCD_FLASH_STRING(title, "GLCD Lib - Box");
    static GLCD_FLASH_STRING(inverted, "Inverted box");

    slide_transition(title);

    x = 2;
    for(i = 2; i < 11; ++i)
    {
        unsigned char height = i * 2;
        unsigned char width = i + 3;
        glcd_draw_box(x, 45 - height, width, height, 1);
        x += 2 + width;
    }

    slide_delay();

    glcd_draw_text_P(2, 13, &proportional_font, inverted);
    glcd_draw_box(11, 14, 60, 32, 2);
}

static void slide_text()
{
    static GLCD_FLASH_STRING(title, "GLCD Lib - Text");
    static GLCD_FLASH_STRING(line1, "- Proportional fonts");
    static GLCD_FLASH_STRING(line2, "- Multiple fonts");

    slide_transition(title);
    glcd_draw_text_P(2, 4, &proportional_font, line1);
    glcd_draw_text_P(3, 4, &proportional_font, line2);
}

int main(void)
{
    glcd_init();

    while(1)
    {
        slide_intro();
        slide_delay();

        slide_pixel();
        slide_delay();

        slide_line();
        slide_delay();
        
        slide_box();
        slide_delay();

        slide_text();
        slide_delay();
    }
}
