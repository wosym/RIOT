#include <avr/pgmspace.h>
#include <stdio.h>

#include "GLCD_SED1531/glcd_io_avrgcc_atmega328p.h"
#include "GLCD_SED1531/glcd.h"
#include "GLCD_SED1531/proportional_font.h"

int main(void)
{
    puts_P(PSTR("Hello World!"));

    printf_P(PSTR("You are running RIOT on a(n) %s board.\n"), RIOT_BOARD);
    printf_P(("This board features a(n) %s MCU.\n"), RIOT_MCU);

    glcd_init();

    glcd_clear_display();
    
    glcd_draw_text(3, 10, &proportional_font, "Hello RIOT!");
    while(1);
    

    return 0;
}
