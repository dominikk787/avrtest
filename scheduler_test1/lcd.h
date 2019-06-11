/*
 * lcd.h
 *
 * Created: 2019-06-08 16:29:32
 *  Author: programowanie
 */ 


#ifndef LCD_H_
#define LCD_H_

#include <avr/io.h>

#define LCD_CLK_SET       PORTD |=  (1<<PD4)
#define LCD_CLK_CLR       PORTD &= ~(1<<PD4)
#define LCD_CLK_SET_OUT   DDRD  |=  (1<<PD4)
/* DATA */
#define LCD_DATA_SET      PORTD |=  (1<<PD3)
#define LCD_DATA_CLR      PORTD &= ~(1<<PD3)
#define LCD_DATA_SET_OUT  DDRD  |=  (1<<PD3)
/* DC */
#define LCD_DC_SET        PORTD |=  (1<<PD2)
#define LCD_DC_CLR        PORTD &= ~(1<<PD2)
#define LCD_DC_SET_OUT    DDRD  |=  (1<<PD2)
/* CE */
#define LCD_CE_SET       PORTD |=  (1<<PD5)
#define LCD_CE_CLR       PORTD &= ~(1<<PD5)
#define LCD_CE_SET_OUT   DDRD  |=  (1<<PD5)

#define LCD_NOP asm volatile("nop\n\t""nop\n\t" "nop\n\t" "nop\n\t" "nop\n\t" "nop\n\t" "nop\n\t" "nop\n\t" ::)
#define LCD_CMD     0
#define LCD_DATA    1
#define LCD_SETX  0x80
#define LCD_SETY  0x40
#define LCD_HOME() lcd_write(LCD_CMD, 0x40);lcd_write(LCD_CMD, 0x80)
#define LCD_GOTO(x,y) lcd_write(LCD_CMD, 0x40|(y));lcd_write(LCD_CMD, 0x80|(x))

void lcd_write( uint8_t cd, uint8_t data);
void lcd_init(void);
void lcd_clear(void);
void lcd_text(char [], uint8_t , uint8_t );
void lcd_image(const uint8_t [],uint8_t , uint8_t , uint8_t , uint8_t );
void lcd_refresh(void);

#endif /* LCD_H_ */