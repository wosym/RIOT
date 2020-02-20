#include "glcd_io_avrgcc_atmega328p.c"
#include "glcd.h"

/* Low level functions */

void glcd_prepare_write_cmd(void)
{
    GLCD_IO_DATA_DIR_OUTPUT();
    GLCD_IO_PIN_RW_0();
    GLCD_IO_PIN_A0_0();
}

void glcd_prepare_write_data(void)
{
    GLCD_IO_DATA_DIR_OUTPUT();
    GLCD_IO_PIN_RW_0();
    GLCD_IO_PIN_A0_1(); 
}

void glcd_prepare_read_data(void)
{
    GLCD_IO_DATA_DIR_INPUT();
    GLCD_IO_PIN_A0_1();
    GLCD_IO_PIN_RW_1();

    /* Dummy read; first read is bogus according to SED1530 datasheet. */
    GLCD_IO_PIN_E_1();
    GLCD_IO_PIN_E_0();
}

void glcd_prepare_read_status(void)
{
    GLCD_IO_DATA_DIR_INPUT();
    GLCD_IO_PIN_A0_0();
    GLCD_IO_PIN_RW_1();
}

void glcd_write(unsigned char data)
{
    GLCD_IO_DATA_OUTPUT(data);
    GLCD_IO_PIN_E_1();
    GLCD_IO_DELAY_WRITE();
    GLCD_IO_PIN_E_0();
}

void glcd_write_repeated(unsigned char data, unsigned char count)
{
    unsigned char i;

    GLCD_IO_DATA_OUTPUT(data);

    for(i = 0; i < count; ++i)
    {
        GLCD_IO_PIN_E_1();
        GLCD_IO_PIN_E_0();
    }
}

void glcd_write_buffer_P(glcd_flash_data_ptr buffer, unsigned char count)
{
    unsigned char i;
    glcd_flash_data_ptr p = buffer;

    for(i = 0; i < count; ++i)
    {
        unsigned char data = GLCD_FLASH_READ_BYTE(p++);
        glcd_write(data);
    }        
}

unsigned char glcd_read(void)
{
    unsigned char data;
    GLCD_IO_PIN_E_0();
    GLCD_IO_PIN_E_1();
    GLCD_IO_DELAY_READ();
    data = GLCD_IO_DATA_INPUT();
    GLCD_IO_PIN_E_0();

    return data;
}

void glcd_update_data(unsigned char pixels, unsigned char mask)
{
    unsigned char data;

    glcd_prepare_read_data();
    data = glcd_read();

    GLCD_IO_DATA_DIR_OUTPUT();
    GLCD_IO_PIN_RW_0();
    glcd_write((data & mask) ^ pixels);
}

void glcd_wait_while_busy(void)
{
    glcd_prepare_read_status();
    while(glcd_read() & GLCD_STATUS_BUSY) 
    {
        /* Do nothing */
    }
}

void glcd_page_update_start(unsigned char page, unsigned char column)
{
    glcd_prepare_write_cmd();
    glcd_write(GLCD_CMD_SET_PAGE(page));
    glcd_write(GLCD_CMD_SET_COLUMN_HIGH(column >> 4));
    glcd_write(GLCD_CMD_SET_COLUMN_LOW(column & 0x0F));
    glcd_write(GLCD_CMD_READ_MODIFY_WRITE);
}

void glcd_page_update_end(void)
{
    glcd_prepare_write_cmd();
    glcd_write(GLCD_CMD_END);
}

/* ---------------------------------------------------------- */

/**
 * Intialize display. This function must be called before any
 * other glcd function.
 */
void glcd_init(void)
{
    GLCD_FLASH(char, init_data[]) =
    {
        GLCD_CMD_ADC_REVERSE,
        GLCD_CMD_LCD_BIAS_1_8,
        GLCD_CMD_POWER_ALL,
        GLCD_CMD_DISPLAY_NORMAL,
        GLCD_CMD_ENTIRE_DISPLAY_NORMAL,
        GLCD_CMD_DISPLAY_ON,
        GLCD_CMD_ELECTRONIC_CONTROL(15),
        GLCD_CMD_INITIAL_DISPLAY_LINE(0)
    };

    GLCD_IO_INIT();

    glcd_wait_while_busy();

    glcd_prepare_write_cmd();
    glcd_write(GLCD_CMD_RESET);

    glcd_wait_while_busy();

    glcd_prepare_write_cmd();
    glcd_write_buffer_P(init_data, sizeof(init_data));
}

/* ---------------------------------------------------------- */

/**
 * Draw a filled rectangle.
 */
void glcd_draw_box(unsigned char left, unsigned char top,
                          unsigned char width, unsigned char height,
                          unsigned char color)
{
    unsigned char x;
    unsigned char mask;
    unsigned char data = 0x00;
    unsigned char page = top / 8;
    unsigned char bit = 1 << (top % 8);

    while(height != 0)
    {
        data |= bit;
        --height;
        bit <<= 1;

        if(bit == 0 || height == 0)
        {
            mask = color != GLCD_COLOR_INVERT ? ~data : 0xFF;

            if(color == GLCD_COLOR_CLEAR)
            {
                data = 0;
            } 
            
            glcd_page_update_start(page, left);
            if(mask == 0x00)
            {
                /* The entire byte is to be overwritten so 
                   don't bother with read-modify-write. */
                glcd_prepare_write_data();
                glcd_write_repeated(data, width);
            }
            else
            {
                for(x = 0; x < width; ++x)
                {
                    glcd_update_data(data, mask);
                }
            }

            glcd_page_update_end();

            ++page;
            data = 0x00;
            bit = 0x01;
        }
    }
}

/**
 * Clear contents of entire display, indicators included.
 */
void glcd_clear_display(void)
{
    glcd_draw_box(0, 0, 132, 64, 0);
}


/**
 * Enable, disable or toggle a single pixel.
 */
void glcd_draw_pixel(unsigned char x, unsigned char y, unsigned char color)
{
   glcd_draw_box(x, y, 1, 1, color);
}


/**
 * Draw unfilled rectangle.
 */
void glcd_draw_frame(unsigned char left, unsigned char top,
                            unsigned char width, unsigned char height,
                            unsigned char color, unsigned char line_width)
{
    glcd_draw_box(left, top, width, line_width, color);
    glcd_draw_box(left, top, line_width, height,color);
    glcd_draw_box(left, top + height - line_width, width, line_width, color);
    glcd_draw_box(left + width - line_width, top, line_width, height, color);
}

/**
 * Draw line from (x1,y1) to (x2,y2) (inclusive).
 */
void glcd_draw_line(unsigned char x1, unsigned char y1,
                           unsigned char x2, unsigned char y2,
                           unsigned char color)
{
    char xinc1;
    char xinc2;
    char yinc1;
    char yinc2;       
    unsigned char deltax;
    unsigned char deltay;
    unsigned char numpixels;
    unsigned char numadd;
    unsigned char num;
    unsigned char den; 
    unsigned char i;

    unsigned char  x = x1;
    unsigned char  y = y1;

    if (x2 >= x1)
    {
        deltax = x2 - x1;
        xinc1 = 1;
    }
    else
    {
        deltax = x1 - x2;
        xinc1 = -1;  
    }

    xinc2 = xinc1;

    if (y2 >= y1)
    {
        deltay = y2 - y1;
        yinc1 = 1;
    }
    else
    {
        deltay = y1 - y2;
        yinc1 = -1;
    }

    yinc2 = yinc1;

    if (deltax >= deltay)
    {  
        xinc1 = 0;
        yinc2 = 0;
        numpixels = deltax;
        numadd = deltay;
    }
    else
    {
        xinc2 = 0;
        yinc1 = 0;
        numpixels = deltay;
        numadd = deltax;
    }

    den = numpixels;
    num = den / 2;

    for (i = 0; i <= numpixels; ++i)
    {
        glcd_draw_pixel(x, y, color);
        num += numadd;
    
        if (num >= den)		 	
        {	
            num -= den;
            x += xinc1;
            y += yinc1;
        }  
    
        x += xinc2;
        y += yinc2;
    }
}


unsigned char glcd_draw_text_int(unsigned char page, 
                                        unsigned char column,
                                        glcd_font_ptr font,
                                        const char* text,
                                        unsigned char text_in_flash)
{
    int i = 0;
    printf("%d\n", i++);
    glcd_page_update_start(page, column);
    printf("%d\n", i++);
    glcd_prepare_write_data();
    printf("%d\n", i++);

    unsigned char first = GLCD_FLASH_READ_BYTE(&(font->first));
    printf("%d\n", i++);
    unsigned char interspacing = GLCD_FLASH_READ_BYTE(&(font->interspacing));
    printf("%d\n", i++);
    const glcd_font_data_ptr* lookup = (const glcd_font_data_ptr*)GLCD_FLASH_READ_WORD(&(font->lookup));
    printf("%d\n", i++);

    unsigned char c = text_in_flash ? GLCD_FLASH_READ_BYTE(text) : *text;
    printf("%d\n", i++);

    while(c != 0)
    {
        unsigned char index = c - first;

        const glcd_font_data_ptr* offset_ptr = lookup + index;

        glcd_flash_data_ptr start = (glcd_flash_data_ptr)GLCD_FLASH_READ_WORD(offset_ptr);
        glcd_flash_data_ptr end = (glcd_flash_data_ptr)GLCD_FLASH_READ_WORD(offset_ptr+1);

        unsigned char width = end - start;
        column += width;

        glcd_write_buffer_P(start, width);
        glcd_write_repeated(0, interspacing);

        ++text;
        c = text_in_flash ? GLCD_FLASH_READ_BYTE(text) : *text;
    }
    printf("%d\n", i++);

    glcd_page_update_end();
    printf("%d\n", i++);

    return column;
}

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
                                    const char* text)
{
    return glcd_draw_text_int(page, column, font, text, 0);
}

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
                                      glcd_flash_text_ptr text)
{
    return glcd_draw_text_int(page, column, font, (const char*)text, 1);
}

/**
 * Enabled, disable or toggle and indicator.
 *
 * @param indicator Indicator to change (use one of the GLCD_INDICATOR_* defines).
 * @param enable    0 = disable, 1 = enable, 2 = toggle.
 */
void glcd_set_indicator(unsigned char indicator, unsigned char enable)
{
    glcd_draw_pixel(indicator, 48, enable);
}

