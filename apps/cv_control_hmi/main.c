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

    xtimer_usleep(100000);
    glcd_clear_display();
    xtimer_usleep(100000);

    glcd_set_indicator(GLCD_INDICATOR_0, 1);
    xtimer_usleep(100000);
    
    glcd_draw_text(3, 10, &proportional_font, "Hello RIOT!");
    xtimer_usleep(100000);
    while(1){
        //puts_P(PSTR("Still working..."));
        glcd_draw_text(3, 10, &proportional_font, ("Hello RIOT!"));
        xtimer_usleep(100000);
        glcd_draw_text_P(3, 10, &proportional_font, ("Hello RIOT!"));
        xtimer_usleep(100000);
    }
    

    return 0;
}
