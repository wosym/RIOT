#ifndef INCLUDED_GLCD_H
#define INCLUDED_GLCD_H

/*
    Target independent implementation of the GLCD SED1531 library.
    The dependent mappings of the I/O pins must be included before this
    file.

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

/* Fallback defines */

#if !defined(GLCD_FLASH)
#error "flash not def!"
#define GLCD_FLASH(type, name) const type name
#endif

#if !defined(GLCD_FLASH_ARRAY)
//#error "flash array not def!"
#define GLCD_FLASH_ARRAY(element_type, name) GLCD_FLASH(element_type, name[])
#endif

#if !defined(GLCD_FLASH_STRING)
//#error "flash string not def!"
#define GLCD_FLASH_STRING(name, value) GLCD_FLASH_ARRAY(char, name) = value
#endif

#if !defined(GLCD_FLASH_PTR)
#error "flash ptr not def!"
#define GLCD_FLASH_PTR(type) const type*
#endif

#if !defined(GLCD_IO_DELAY_READ)
#error "delay read not def!"
#define GLCD_IO_DELAY_READ()
#endif

#if !defined(GLCD_IO_DELAY_WRITE)
//#error "delay write not def!"
#define GLCD_IO_DELAY_WRITE()
#endif

//Turned into a function!
//#if !defined(GLCD_IO_INIT)
//#error "glcd_io_init not def!!"
//#define GLCD_IO_INIT()
//#endif

/* Types */
typedef GLCD_FLASH_PTR(char) glcd_font_data_ptr;

typedef struct glcd_font_struct
{
    unsigned char first;
    unsigned char interspacing;
    GLCD_FLASH_PTR(glcd_font_data_ptr) lookup;
} glcd_font_t;

typedef GLCD_FLASH_PTR(glcd_font_t) glcd_font_ptr;

typedef GLCD_FLASH_PTR(char) glcd_flash_data_ptr;
typedef GLCD_FLASH_PTR(char) glcd_flash_text_ptr;


/* GLCD Commands */
#define GLCD_CMD_DISPLAY_OFF                    (0xAE)
#define GLCD_CMD_DISPLAY_ON                     (0xAF)
#define GLCD_CMD_INITIAL_DISPLAY_LINE(start)    (0x40 | (start))
#define GLCD_CMD_SET_PAGE(page)                 (0xB0 | (page))
#define GLCD_CMD_SET_COLUMN_HIGH(col_high)      (0x10 | (col_high))     
#define GLCD_CMD_SET_COLUMN_LOW(col_low)        (col_low)
#define GLCD_CMD_ADC_NORMAL                     (0xA0)     
#define GLCD_CMD_ADC_REVERSE                    (0xA1)     
#define GLCD_CMD_READ_MODIFY_WRITE              (0xE0)
#define GLCD_CMD_DISPLAY_NORMAL                 (0xA6)
#define GLCD_CMD_DISPLAY_REVERSE                (0xA7)
#define GLCD_CMD_ENTIRE_DISPLAY_NORMAL          (0xA4)
#define GLCD_CMD_ENTIRE_DISPLAY_ON              (0xA5)
#define GLCD_CMD_LCD_BIAS_1_8                   (0xA2)
#define GLCD_CMD_LCD_BIAS_1_6                   (0xA3)
#define GLCD_CMD_READ_MODIFY_WRITE              (0xE0)
#define GLCD_CMD_END                            (0xEE)
#define GLCD_CMD_RESET                          (0xE2)
#define GLCD_CMD_POWER_NONE                     (0x28)
#define GLCD_CMD_POWER_FOLLOWER                 (0x29)
#define GLCD_CMD_POWER_REGULATOR                (0x2A)
#define GLCD_CMD_POWER_BOOSTER                  (0x2C)
#define GLCD_CMD_POWER_ALL                      (0x2F)
#define GLCD_CMD_ELECTRONIC_CONTROL(voltage)    (0x80 | (voltage))

/* GLCD Status bits */
#define GLCD_STATUS_BUSY                        (0x80)
#define GLCD_STATUS_ADC                         (0x40)
#define GLCD_STATUS_ON_OFF                      (0x20)
#define GLCD_STATUS_RESET                       (0x10)

/* Colors */
#define GLCD_COLOR_CLEAR                        (0x00)
#define GLCD_COLOR_SET                          (0x01)
#define GLCD_COLOR_INVERT                       (0x02)

/* Indicators */
#define GLCD_INDICATOR_0                        (20)
#define GLCD_INDICATOR_1                        (31)
#define GLCD_INDICATOR_2                        (32)
#define GLCD_INDICATOR_3                        (57)
#define GLCD_INDICATOR_4                        (69)
#define GLCD_INDICATOR_5                        (78)


/* Low level functions */

void glcd_prepare_write_cmd(void);

void glcd_prepare_write_data(void);

void glcd_prepare_read_data(void);

void glcd_prepare_read_status(void);

void glcd_write(unsigned char data);

void glcd_write_repeated(unsigned char data, unsigned char count);

void glcd_write_buffer_P(glcd_flash_data_ptr buffer, unsigned char count);

unsigned char glcd_read(void);

void glcd_update_data(unsigned char pixels, unsigned char mask);

void glcd_wait_while_busy(void);

void glcd_page_update_start(unsigned char page, unsigned char column);

void glcd_page_update_end(void);

/* ---------------------------------------------------------- */

/**
 * Intialize display. This function must be called before any
 * other glcd function.
 */
void glcd_init(void);

/* ---------------------------------------------------------- */

/**
 * Draw a filled rectangle.
 */
void glcd_draw_box(unsigned char left, unsigned char top,
                          unsigned char width, unsigned char height,
                          unsigned char color);

/**
 * Clear contents of entire display, indicators included.
 */
void glcd_clear_display(void);

/**
 * Enable, disable or toggle a single pixel.
 */
void glcd_draw_pixel(unsigned char x, unsigned char y, unsigned char color);

/**
 * Draw unfilled rectangle.
 */
void glcd_draw_frame(unsigned char left, unsigned char top,
                            unsigned char width, unsigned char height,
                            unsigned char color, unsigned char line_width);

/**
 * Draw line from (x1,y1) to (x2,y2) (inclusive).
 */
void glcd_draw_line(unsigned char x1, unsigned char y1,
                           unsigned char x2, unsigned char y2,
                           unsigned char color);

unsigned char glcd_draw_text_int(unsigned char page, 
                                        unsigned char column,
                                        glcd_font_ptr font,
                                        const char* text,
                                        unsigned char text_in_flash);

/**
 * Draw text stored in RAM.
 *
 * @param page      Line at which text should be written.
 * @param column    Pixel column at which text should be written.
 * @param font      Font to use for text.
 * @param text      RAM memory pointer to text.
 *
 * @return Pixel column where text ends.
 */
unsigned char glcd_draw_text(unsigned char page, 
                                    unsigned char column,
                                    glcd_font_ptr font,
                                    const char* text);
/**
 * Draw text stored in program memory.
 *
 * This function is added for Harvard architecture processors,
 * such as the Atmel 8-bit AVR series, to avoid having to use
 * precious RAM memory for fixed texts.
 *
 * @param page      Line at which text should be written.
 * @param column    Pixel column at which text should be written.
 * @param font      Font to use for text.
 * @param text      Program memory pointer to text.
 *
 * @return Pixel column where text ends.
 */
unsigned char glcd_draw_text_P(unsigned char page, 
                                      unsigned char column,
                                      glcd_font_ptr font,
                                      glcd_flash_text_ptr text);
/**
 * Enabled, disable or toggle and indicator.
 *
 * @param indicator Indicator to change (use one of the GLCD_INDICATOR_* defines).
 * @param enable    0 = disable, 1 = enable, 2 = toggle.
 */
void glcd_set_indicator(unsigned char indicator, unsigned char enable);
#endif /* #ifndef INCLUDED_GLCD_H */
