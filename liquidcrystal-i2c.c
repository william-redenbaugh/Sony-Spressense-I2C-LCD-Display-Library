/**
 * Heavily inspired by Arduino-LiquidCrystalI2C-library by fdebrabander on github
 * Visit the original library here: https://github.com/fdebrabander/Arduino-LiquidCrystal-I2C-library/
*/
#include "liquidcrystal-i2c.h"
#include "string.h"
#include <stdio.h>
#include <nuttx/config.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <debug.h>
#include <nuttx/i2c/i2c_master.h>
#include "assert.h"

// When the display powers up, it is configured as follows:
//
// 1. Display clear
// 2. Function set:
//    DL = 1; 8-bit interface data
//    N = 0; 1-line display
//    F = 0; 5x8 dot character font
// 3. Display on/off control:
//    D = 0; Display off
//    C = 0; Cursor off
//    B = 0; Blinking off
// 4. Entry mode set:
//    I/D = 1; Increment by 1
//    S = 0; No shift
//
// Note, however, that resetting the Arduino doesn't reset the LCD, so we
// can't assume that its in that state when a sketch starts (and the
// LiquidCrystal constructor is called).

static inline void i2c_init(LiquidCrystalI2C_t *display){
    int fd;

    /* Open the i2c driver */
    printf("Initializing i2c device...\n");
    fd = open("/dev/i2c0", O_WRONLY);
    if (fd < 0)
    {
    printf("ERROR: failed to open /dev/i2c0: %d\n", fd);
    return ERROR;
    }
    else
    {
    printf("i2c init successful!\n");
    }

    display->i2c_handler = fd;
}

static void i2c_expander_write(LiquidCrystalI2C_t *display, uint8_t data){
    int16_t ex;
    struct i2c_msg_s i2c_msg;
    struct i2c_transfer_s i2c_transfer;
    uint8_t tx_buff[1];

    // Pack up the data
    tx_buff[0] = (data | display->_backlightval);

    i2c_msg.addr = display->_addr;
    i2c_msg.buffer = tx_buff;
    i2c_msg.length = 1;
    i2c_msg.flags = I2C_M_NOSTART;
    i2c_msg.frequency = I2C_SPEED_FAST;

    i2c_transfer.msgv = &i2c_msg;
    i2c_transfer.msgc = 1;
    int transfer_result = ioctl(display->i2c_handler, I2CIOC_TRANSFER, (unsigned long)(uintptr_t)&i2c_transfer);

    if (transfer_result < 0){
        printf("i2c Write Error: Couldn't write to LED Display correctly...\n");
        check_err_i2c(transfer_result);
    }

}

static inline void i2c_pulse_enable(LiquidCrystalI2C_t *display, uint8_t _data){
    i2c_expander_write(display, _data | display->_backlightval);
    usleep(1);
    i2c_expander_write(display, _data & display->_backlightval);
    usleep(50);
}

static inline void i2c_write4bits(LiquidCrystalI2C_t *display, uint8_t value){
    i2c_expander_write(display, value);
    i2c_pulse_enable(display, value);
}

static inline void i2c_send(LiquidCrystalI2C_t *display, uint8_t value, uint8_t mode){
    uint8_t highnib=value&0xf0;
	uint8_t lownib=(value<<4)&0xf0;
    i2c_write4bits(display, highnib | mode);
    i2c_write4bits(display, lownib | mode);
}

static inline void i2c_command(LiquidCrystalI2C_t *display, uint8_t value){
    i2c_send(display, value, 0);
}

static inline void liquidcrystal_i2c_write(LiquidCrystalI2C_t *display, uint8_t value){
    i2c_send(display, value, Rs);
}

LiquidCrystalI2C_t new_liquidcrystal_i2c(uint8_t lcd_addr, uint8_t lcd_cols, uint8_t lcd_rows, uint8_t charsize){
    LiquidCrystalI2C_t crystal;
    crystal._addr = lcd_addr;
    crystal._cols = lcd_cols;
    crystal._rows = lcd_rows;
    crystal._charsize = charsize;
    crystal._backlightval = LCD_BACKLIGHT;
    return crystal;
}

void liquidcrystal_i2c_begin(LiquidCrystalI2C_t *display){
    i2c_init(display);

    display->_displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;
    if(display->_rows > 1)
        display->_displayfunction |= LCD_2LINE;

    // for some 1 line displays you can select a 10 pixel high font
	if ((display->_charsize != 0) && (display->_rows == 1)) {
		display->_displayfunction |= LCD_5x10DOTS;
	}

    // Sleep... figure out if this is needed soon
    usleep(50000);

    i2c_expander_write(display, display->_backlightval);
    usleep(1000000);

    // Attempt to get I2C Module into 4bit mode
    i2c_write4bits(display, 0x03 << 4);
    usleep(4500);
    i2c_write4bits(display, 0x03 << 4);
    usleep(4500);
    i2c_write4bits(display, 0x03 << 4);
    usleep(4500);

    // Set to 4bit interface
    i2c_write4bits(display, 0x02 << 4);

    // set # lines, font size, etc.
	i2c_command(display, LCD_FUNCTIONSET | display->_displayfunction);

	// turn the display on with no cursor or blinking default
	display->_displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
	liquidcrystal_i2c_display(display);

	// clear it off
	liquidcrystal_i2c_clear(display);

	// Initialize to default text direction (for roman languages)
	display->_displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;

	// set the entry mode
	i2c_command(display, LCD_ENTRYMODESET | display->_displaymode);
    liquidcrystal_i2c_home(display);
}

void liquidcrystal_i2c_home(LiquidCrystalI2C_t *display){
    i2c_command(display, LCD_RETURNHOME);
    usleep(2000);
}

void liquidcrystal_i2c_no_display(LiquidCrystalI2C_t *display){
    display->_displaycontrol &= ~LCD_DISPLAYON;
	i2c_command(display, LCD_DISPLAYCONTROL | display->_displaycontrol);
}

void liquidcrystal_i2c_display(LiquidCrystalI2C_t *display){
    display->_displaycontrol |= LCD_DISPLAYON;
	i2c_command(display, LCD_DISPLAYCONTROL | display->_displaycontrol);
}

void liquidcrystal_i2c_no_blink(LiquidCrystalI2C_t *display){
    display->_displaycontrol &= ~LCD_BLINKON;
	i2c_command(display, LCD_DISPLAYCONTROL | display->_displaycontrol);
}

void liquidcrystal_i2c_blink(LiquidCrystalI2C_t *display){
    display->_displaycontrol |= LCD_BLINKON;
	command(LCD_DISPLAYCONTROL | display->_displaycontrol);
}

void liquidcrystal_i2c_no_cursor(LiquidCrystalI2C_t *display){
    display->_displaycontrol &= ~LCD_CURSORON;
	command(LCD_DISPLAYCONTROL | display->_displaycontrol);
}

void liquidcrystal_i2c_cursor(LiquidCrystalI2C_t *display){
    display->_displaycontrol |= LCD_CURSORON;
	command(LCD_DISPLAYCONTROL | display->_displaycontrol);
}

void liquidcrystal_i2c_backlight(LiquidCrystalI2C_t *display){
    display->_backlightval=LCD_BACKLIGHT;
	i2c_expander_write(display, 0);
}

void liquidcrystal_i2c_no_backlight(LiquidCrystalI2C_t *display){
    display->_backlightval=LCD_BACKLIGHT;
	i2c_expander_write(display, 0);
}

bool liquidcrystal_i2c_get_backlight(LiquidCrystalI2C_t *display){
    return (display->_backlightval == LCD_BACKLIGHT);
}

void liquidcrystal_i2c_clear(LiquidCrystalI2C_t *display){
    i2c_command(display, LCD_CLEARDISPLAY);
    usleep(2000);
}


void liquidcrystal_i2c_autoscroll(LiquidCrystalI2C_t *display){
    display->_displaymode |= LCD_ENTRYSHIFTINCREMENT;
	i2c_command(display, LCD_ENTRYMODESET | display->_displaymode);
}

void liquidcrystal_i2c_no_autoscroll(LiquidCrystalI2C_t *display){
    display->_displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
	i2c_command(display, LCD_ENTRYMODESET | display->_displaymode);
}

void liquidcrystal_i2c_create_char(LiquidCrystalI2C_t *display, uint8_t location, uint8_t charmap[]){
    location &= 0x7; // we only have 8 locations 0-7
	command(LCD_SETCGRAMADDR | (location << 3));
	for (int i=0; i<8; i++)
        liquidcrystal_i2c_write(display, charmap[i]);
}

void liquidcrystal_i2c_set_cursor(LiquidCrystalI2C_t *display, uint8_t col, uint8_t row){
    int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
	if (row > display->_rows) {
		row = display->_rows-1;    // we count rows starting w/0
	}
	i2c_command(display, LCD_SETDDRAMADDR | (col + row_offsets[row]));
}