/***********************************
Author: Ahnaf Zareef

Minimal ILI9341 C Driver for FPGA Support
on MicroBlaze Systems.

*************************************/

#ifndef _ILI9341_XIL_H_
#define _ILI9341_XIL_H_

// Xilinx Driver APIs for SPI and GPIO.
#include "xspi.h"
#include "xgpio.h"
#include <stdint.h>
#include <xil_types.h>

// Display Dimensions
#define ILI9341_WIDTH 240
#define ILI9341_HEIGHT 320

// Colour Definitions (16b | 2 Byte (hi, lo))
// 16 Bit: 0xF800 -> 1111 1000 0000 0000
// Where:            RRRR RGGG GGGB BBBB (Green is most perceptible)

#define ILI9341_BLACK 0x0000
#define ILI9341_NAVY 0x000F
#define ILI9341_DARKGREEN 0x03E0
#define ILI9341_DARKCYAN 0x03EF
#define ILI9341_MAROON 0x7800
#define ILI9341_PURPLE 0x780F
#define ILI9341_OLIVE 0x7BE0
#define ILI9341_LIGHTGREY 0xC618
#define ILI9341_DARKGREY 0x7BEF
#define ILI9341_BLUE 0x001F
#define ILI9341_GREEN 0x07E0
#define ILI9341_CYAN 0x07FF
#define ILI9341_RED 0xF800
#define ILI9341_MAGENTA 0xF81F
#define ILI9341_YELLOW 0xFFE0
#define ILI9341_WHITE 0xFFFF
#define ILI9341_ORANGE 0xFD20
#define ILI9341_GREENYELLOW 0xAFE5
#define ILI9341_PINK 0xF81F

// Store the states
typedef struct
{
    XSpi *spi;
    XGpio *gpio;
    unsigned gpio_channel;
    u32 rst_mask, dc_mask;
    uint16_t height, width;
} ili9341_t;

// Member Functions

// Pass Display Dimensions
//*spi/*gpio -> pointer to spi/gpio features (address, config, state)
void ili9341_init(ili9341_t *display, XSpi *Spi, XGpio *Gpio, u32 rst_mask, u32 dc_mask, unsigned gpio_channel); // 0x01 is rst_mask, 0x02 is dc_max. and 1 is gpio channel.

// Eventually Add the RGB Integer (0-255) to 16 bit colour handling

void ili9341_fill_screen(ili9341_t *display, uint16_t colour);
void ili9341_fill_rect(ili9341_t *display, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t colour);

void ili9341_drawImage(ili9341_t *display, uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t *image);

void setPinHi(ili9341_t *display, u32 mask);
void setPinLo(ili9341_t *display, u32 mask);

// Send a command to the display or data. DC pin low for command, high for data.
void writeCommand(ili9341_t *display, uint8_t command);
void writeData(ili9341_t *display, uint8_t data);

// Set drawing window. (Start:x0/y0, End:x1/y1)
void setAddressWindow(ili9341_t *display, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void writePixel(ili9341_t *display, uint16_t colour);                        // Sending a Colour to the address window.
void drawPixel(ili9341_t *display, uint16_t x, uint16_t y, uint16_t colour); // Draw one pixel of any colour
void initDisplay(ili9341_t *display);                                        // Startup Sequence.
void ili9341_setRotation(ili9341_t *display, uint8_t rotation);
void ili9341_reset(ili9341_t *display);                     // Hardware Reset of Display
void ili9341_invertDisplay(ili9341_t *display, int invert); // Invert Display On/Off 1 is on, 0 is off.
void ili9341_drawChar(ili9341_t *display, char character, uint8_t x, uint8_t y, uint16_t colour, uint8_t size, uint16_t bg);
void ili9341_drawText(ili9341_t *display, const char *text, uint8_t x, uint8_t y, uint16_t colour, uint8_t size, uint16_t bg);

#endif //_ILI9341_XIL_H_
