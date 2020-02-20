#include <avr/pgmspace.h>
#include <stdio.h>
#include <xtimer.h>

#include "glcd_io_avrgcc_atmega328p.h"
#include "glcd.h"
#include "proportional_font.h"

int main(void)
{
    puts_P(PSTR("Hello World!"));

    printf_P(PSTR("You are running RIOT on a(n) %s board.\n"), RIOT_BOARD);
    printf_P(("This board features a(n) %s MCU.\n"), RIOT_MCU);

    glcd_init();

    glcd_clear_display();

    glcd_set_indicator(GLCD_INDICATOR_0, 1);
    
    //glcd_draw_text_P(3, 10, &proportional_font, PSTR("Hello RIOT!"));
    while(1){
        //puts_P(PSTR("Still working..."));
        //glcd_draw_text(3, 10, &proportional_font, ("Hello RIOT!"));
    }
    

    return 0;
}
