/**
 * Heavily inspired by Arduino-LiquidCrystalI2C-library by fdebrabander on github
 * Visit the original library here: https://github.com/fdebrabander/Arduino-LiquidCrystal-I2C-library/
*/

#ifndef _LIQUIDCRYSTAL_I2C
#define _LIQUIDCRYSTAL_I2C
#include "stdio.h"

// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

// flags for backlight control
#define LCD_BACKLIGHT 0x08
#define LCD_NOBACKLIGHT 0x00

#define En 0B00000100  // Enable bit
#define Rw 0B00000010  // Read/Write bit
#define Rs 0B00000001  // Register select bit

typedef struct LiquidCrystalI2C{
    uint8_t _addr;
	uint8_t _displayfunction;
	uint8_t _displaycontrol;
	uint8_t _displaymode;
	uint8_t _cols;
	uint8_t _rows;
	uint8_t _charsize;
	uint8_t _backlightval;
    int i2c_handler;
}LiquidCrystalI2C_t;

/**
 * Constructor
 *
 * @param lcd_addr	I2C slave address of the LCD display. Most likely printed on the
 *					LCD circuit board, or look in the supplied LCD documentation
 * @param lcd_cols	Number of columns your LCD display has.
 * @param lcd_rows	Number of rows your LCD display has.
 * @param charsize	The size in dots that the display has, use LCD_5x10DOTS or LCD_5x8DOTS.
*/
LiquidCrystalI2C_t new_liquidcrystal_i2c(uint8_t lcd_addr, uint8_t lcd_cols, uint8_t lcd_rows, uint8_t charsize);

/**
 * @brief Set the LCD display in the correct begin state, must be called before anything else is done.
*/
void liquidcrystal_i2c_begin(LiquidCrystalI2C_t *display);

/**
 * @brief Next print/write operation will will start from the first position on the LCD display.
 */
void liquidcrystal_i2c_home(LiquidCrystalI2C_t *display);

/**
 * @brief Do not show any characters on the LCD display. Backlight state will remain unchanged.
 * Also all characters written on the display will return, when the display in enabled again.
 */
void liquidcrystal_i2c_no_display(LiquidCrystalI2C_t *display);

/**
 * @brief Show the characters on the LCD display, this is the normal behaviour. This method should
 * only be used after noDisplay() has been used.
 */
void liquidcrystal_i2c_display(LiquidCrystalI2C_t *display);

/**
 * @brief Do not blink the cursor indicator.
 */
void liquidcrystal_i2c_no_blink(LiquidCrystalI2C_t *display);

/**
 * @brief Start blinking the cursor indicator.
 */
void liquidcrystal_i2c_blink(LiquidCrystalI2C_t *display);

/**
 * @brief Do not show a cursor indicator.
 */
void liquidcrystal_i2c_no_cursor(LiquidCrystalI2C_t *display);

/**
 * @brief Show a cursor indicator, cursor can blink on not blink. Use the
 * methods blink() and noBlink() for changing cursor blink.
 */
void liquidcrystal_i2c_cursor(LiquidCrystalI2C_t *display);

/**
 * @brief Remove all the characters currently shown. Next print/write operation will start
 * from the first position on LCD display.
 */
void liquidcrystal_i2c_clear(LiquidCrystalI2C_t *display);

void liquidcrystal_i2c_backlight(LiquidCrystalI2C_t *display);
void liquidcrystal_i2c_no_backlight(LiquidCrystalI2C_t *display);
bool liquidcrystal_i2c_get_backlight(LiquidCrystalI2C_t *display);
void liquidcrystal_i2c_autoscroll(LiquidCrystalI2C_t *display);
void liquidcrystal_i2c_no_autoscroll(LiquidCrystalI2C_t *display);
void liquidcrystal_i2c_create_char(LiquidCrystalI2C_t *display, uint8_t location, uint8_t charmap[]);
void liquidcrystal_i2c_set_cursor(LiquidCrystalI2C_t *display, uint8_t col, uint8_t row);
#endif