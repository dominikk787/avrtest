/*
 * n5110_test0.c
 *
 * Created: 2019-06-08 14:54:21
 * Author : programowanie
 */ 

#include <avr/io.h>
#include "lcd.h"

int main(void)
{
	lcd_init();
	lcd_clear();
	lcd_text("Kurs AVR-GCC", 1*6, 0);
	lcd_text("ABXYZ :)", 4*6, 1);
	lcd_text("Wyœwietlacz", 1*6, 3);
	lcd_text("LCD", 5*6, 4);
	lcd_text("od Nokii 5110", 1, 5);
    while (1) 
    {
    }
}

