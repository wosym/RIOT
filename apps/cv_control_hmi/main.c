#include <avr/pgmspace.h>
#include <stdio.h>
#include <xtimer.h>

#include "glcd_io_avrgcc_atmega328p.h"
#include "glcd.h"
#include "proportional_font.h"

#define xtimer_msleep(t) (xtimer_usleep((uint32_t)t * 1000))

/*
void xtimer_msleep(uint32_t t) {

        printf("Sleeping for %d\n", t * 1000);
        xtimer_usleep(t * 1000);
}
*/

int main(void)
{
    puts_P(PSTR("Hello World!"));

    printf_P(PSTR("You are running RIOT on a(n) %s board.\n"), RIOT_BOARD);
    printf_P(("This board features a(n) %s MCU.\n"), RIOT_MCU);

    glcd_init();
    xtimer_msleep(600);

    
    while(1){
        puts("Abcd");
        xtimer_msleep(600);
        glcd_clear_display();
        xtimer_msleep(600);
        glcd_draw_text(0, 20, &proportional_font, "Hello RIOT!");
        xtimer_msleep(600);

        glcd_set_indicator(GLCD_INDICATOR_0, 1);
        xtimer_msleep(600);
        glcd_set_indicator(GLCD_INDICATOR_1, 1);
        xtimer_msleep(600);
        glcd_set_indicator(GLCD_INDICATOR_2, 1);
        xtimer_msleep(600);
        glcd_set_indicator(GLCD_INDICATOR_3, 1);
        xtimer_msleep(600);
        glcd_set_indicator(GLCD_INDICATOR_4, 1);
        xtimer_msleep(600);
        glcd_set_indicator(GLCD_INDICATOR_5, 1);
        xtimer_msleep(600);
        glcd_draw_text_P(1, 0, &proportional_font, PSTR("Also works with "));
        xtimer_msleep(600);
        glcd_draw_text_P(2, 20, &proportional_font, PSTR("Flash memory ;) "));
        xtimer_msleep(600);
    }
    

    return 0;
}
