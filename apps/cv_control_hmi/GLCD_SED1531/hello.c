// #include "glcd_io_avrgcc_attiny2313.h"
#include "glcd_io_avrgcc_atmega328p.h"
#include "glcd.h"
#include "proportional_font.h"
//#define F_CPU 16000000UL	//Deze moet blijkbaar altijd VOOR de import van delay staan!!
#define F_CPU 8000000UL	//Deze moet blijkbaar altijd VOOR de import van delay staan!!
#include <util/delay.h>    



int main()
{
   glcd_init();
   while(1){

	   glcd_clear_display();
	   glcd_draw_text(3, 10, &proportional_font, "Hello world!");

	   _delay_ms(500/4);
	   glcd_clear_display();

	   glcd_draw_box(0, 0, 95, 35, 1);
	   glcd_draw_box(2, 2, 91, 31, 0);
   	   glcd_draw_text(3, 5, &proportional_font, "Dit is een test!");

   	   _delay_ms(500/4);
   		glcd_clear_display();
	   glcd_draw_frame(2, 2, 90, 30, 1, 5);


	   _delay_ms(100/4);
	   glcd_set_indicator(GLCD_INDICATOR_0, 1);
	   _delay_ms(100/4);
	   glcd_set_indicator(GLCD_INDICATOR_1, 1);
	   _delay_ms(100/4);
	   glcd_set_indicator(GLCD_INDICATOR_2, 1);
	   _delay_ms(100/4);
	   glcd_set_indicator(GLCD_INDICATOR_3, 1);
	   _delay_ms(100/4);
	   glcd_set_indicator(GLCD_INDICATOR_4, 1);
	   _delay_ms(100/4);
	   glcd_set_indicator(GLCD_INDICATOR_5, 1);


   	   _delay_ms(2000/4);

   }
}
