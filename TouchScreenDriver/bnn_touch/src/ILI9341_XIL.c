#include "ILI9341_XIL.h"
#include <stdio.h>
#include "sleep.h"
#include <stdlib.h>
#include "55font.h"

void ili9341_init(ili9341_t *display, XSpi *Spi, XGpio *Gpio, u32 rst_mask, u32 dc_mask, unsigned gpio_channel)
{
    // Update Variable States
    display->spi = Spi;
    display->gpio = Gpio;
    display->rst_mask = rst_mask;
    display->dc_mask = dc_mask;
    display->gpio_channel = gpio_channel;
    display->height = ILI9341_HEIGHT;
    display->width = ILI9341_WIDTH;

    if (Spi->IsStarted != XIL_COMPONENT_IS_STARTED)
        XSpi_Start(Spi);

    // Turn on SPI Transmitter. By default AXI Quad has transmitter off. Turn on
    // The Control Register has bit 8 as the Inhibit.
    u32 controlRegister = XSpi_GetControlReg(Spi);
    controlRegister &= ~(1 << 8); // Clearing Bit 8.
    XSpi_SetControlReg(Spi, controlRegister);

    initDisplay(display);
} // ili9341_init

// For Filling WHOLE Screen.
void ili9341_fill_screen(ili9341_t *display, uint16_t colour)
{
    ili9341_fill_rect(display, 0, 0, display->width, display->height, colour);

} // ili9341_fill_screen

void ili9341_drawImage(ili9341_t *display, uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t *image)
{
    // RGB565 for ILI9341

    setAddressWindow(display, x, y, x + w - 1, y + h - 1);
    setPinHi(display, display->dc_mask); // Data Mode

    // Send Image data
    uint8_t buffer[ILI9341_WIDTH * 2]; // hold one row of image which is 2 byte per pixel
    uint32_t count = 0;

    for (uint16_t row = 0; row < h; row++)
    {
        uint16_t pixelCount = 0;
        for (uint16_t col = 0; col < w; col++)
        {
            uint16_t colour = image[count++];     // one 16 bit pixel
            buffer[pixelCount++] = colour >> 8;   // hi byte
            buffer[pixelCount++] = colour & 0xFF; // lo byte
        }
        XSpi_Transfer(display->spi, buffer, NULL, pixelCount);
    }
}
void setPinHi(ili9341_t *display, u32 mask)
{
    XGpio_DiscreteSet(display->gpio, display->gpio_channel, mask);
}

void setPinLo(ili9341_t *display, u32 mask)
{
    XGpio_DiscreteClear(display->gpio, display->gpio_channel, mask);
}

void writeCommand(ili9341_t *display, uint8_t command)
{
    setPinLo(display, display->dc_mask); // Command Mode
    XSpi_Transfer(display->spi, &command, NULL, 1);
}

void writeData(ili9341_t *display, uint8_t data)
{
    setPinHi(display, display->dc_mask); // Data Mode
    XSpi_Transfer(display->spi, &data, NULL, 1);
}

void setAddressWindow(ili9341_t *display, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    writeCommand(display, 0x2A);         // Column Address Set
    setPinHi(display, display->dc_mask); // Data Mode, next bytes read as data.

    // send the four coordinates, x0,y0,x1,y1
    uint8_t data[4];
    data[0] = x0 >> 8;   // hi byte of x0
    data[1] = x0 & 0xFF; // lo byte of x0
    data[2] = x1 >> 8;   // hi byte of x1
    data[3] = x1 & 0xFF; // lo byte of x1

    XSpi_Transfer(display->spi, data, NULL, 4);

    writeCommand(display, 0x2B); // Rows
    setPinHi(display, display->dc_mask);

    data[0] = y0 >> 8;   // hi byte of y0
    data[1] = y0 & 0xFF; // lo byte of y0
    data[2] = y1 >> 8;   // hi byte of y1
    data[3] = y1 & 0xFF; // lo byte of y1
    XSpi_Transfer(display->spi, data, NULL, 4);

    writeCommand(display, 0x2C); // Memory Write
} // setAddressWindow

void ili9341_fill_rect(ili9341_t *display, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t colour)
{
    setAddressWindow(display, x, y, x + w - 1, y + h - 1);
    setPinHi(display, display->dc_mask); // Data Mode

    int totalPixels = w * h;

    u8 hi = colour >> 8;   // hi
    u8 lo = colour & 0xFF; // low

    for (int i = 0; i < totalPixels; i++)
    {
        XSpi_Transfer(display->spi, &hi, NULL, 1);
        XSpi_Transfer(display->spi, &lo, NULL, 1);
    }
}

// write pixel is to set colour wherever the address window is.
void writePixel(ili9341_t *display, uint16_t colour)
{
    setPinHi(display, display->dc_mask); // Data Mode

    u8 hi = colour >> 8;   // hi
    u8 lo = colour & 0xFF; // low

    XSpi_Transfer(display->spi, &hi, NULL, 1);
    XSpi_Transfer(display->spi, &lo, NULL, 1);
} // writePixel

// draw pixel draws at this coordinate.
void drawPixel(ili9341_t *display, uint16_t x, uint16_t y, uint16_t colour)
{
    setAddressWindow(display, x, y, x, y); // Set to one pixel
    writePixel(display, colour);
}

void ili9341_reset(ili9341_t *display)
{
    // Reset the display
    setPinHi(display, display->rst_mask);
    usleep(20000); // 20ms
    setPinLo(display, display->rst_mask);
    usleep(20000); // 20ms
    setPinHi(display, display->rst_mask);
    usleep(150000); // 150ms
}

void initDisplay(ili9341_t *display)
{

    // Need Hardware Reset + software reset before init.
    ili9341_reset(display);

    writeCommand(display, 0x01); // Software Reset
    usleep(150000);              // 150ms

    // init sequence (From STM32 ILI9341 Driver Library)

    // POWER CONTROL A
    writeCommand(display, 0xCB);
    writeData(display, 0x39);
    writeData(display, 0x2C);
    writeData(display, 0x00);
    writeData(display, 0x34);
    writeData(display, 0x02);

    // POWER CONTROL B
    writeCommand(display, 0xCF);
    writeData(display, 0x00);
    writeData(display, 0xC1);
    writeData(display, 0x30);

    // DRIVER TIMING CONTROL A
    writeCommand(display, 0xE8);
    writeData(display, 0x85);
    writeData(display, 0x00);
    writeData(display, 0x78);

    // DRIVER TIMING CONTROL B
    writeCommand(display, 0xEA);
    writeData(display, 0x00);
    writeData(display, 0x00);

    // POWER ON SEQUENCE CONTROL
    writeCommand(display, 0xED);
    writeData(display, 0x64);
    writeData(display, 0x03);
    writeData(display, 0x12);
    writeData(display, 0x81);

    // PUMP RATIO CONTROL
    writeCommand(display, 0xF7);
    writeData(display, 0x20);

    // POWER CONTROL,VRH[5:0]
    writeCommand(display, 0xC0);
    writeData(display, 0x23);

    // POWER CONTROL,SAP[2:0];BT[3:0]
    writeCommand(display, 0xC1);
    writeData(display, 0x10);

    // VCM CONTROL
    writeCommand(display, 0xC5);
    writeData(display, 0x3E);
    writeData(display, 0x28);

    // VCM CONTROL 2
    writeCommand(display, 0xC7);
    writeData(display, 0x86);

    // MEMORY ACCESS CONTROL
    writeCommand(display, 0x36);
    writeData(display, 0x48);

    // PIXEL FORMAT
    writeCommand(display, 0x3A);
    writeData(display, 0x55);

    // FRAME RATIO CONTROL, STANDARD RGB COLOR
    writeCommand(display, 0xB1);
    writeData(display, 0x00);
    writeData(display, 0x18);

    // DISPLAY FUNCTION CONTROL
    writeCommand(display, 0xB6);
    writeData(display, 0x08);
    writeData(display, 0x82);
    writeData(display, 0x27);

    // 3GAMMA FUNCTION DISABLE
    writeCommand(display, 0xF2);
    writeData(display, 0x00);

    // GAMMA CURVE SELECTED
    writeCommand(display, 0x26);
    writeData(display, 0x01);

    // POSITIVE GAMMA CORRECTION
    writeCommand(display, 0xE0);
    writeData(display, 0x0F);
    writeData(display, 0x31);
    writeData(display, 0x2B);
    writeData(display, 0x0C);
    writeData(display, 0x0E);
    writeData(display, 0x08);
    writeData(display, 0x4E);
    writeData(display, 0xF1);
    writeData(display, 0x37);
    writeData(display, 0x07);
    writeData(display, 0x10);
    writeData(display, 0x03);
    writeData(display, 0x0E);
    writeData(display, 0x09);
    writeData(display, 0x00);

    // NEGATIVE GAMMA CORRECTION
    writeCommand(display, 0xE1);
    writeData(display, 0x00);
    writeData(display, 0x0E);
    writeData(display, 0x14);
    writeData(display, 0x03);
    writeData(display, 0x11);
    writeData(display, 0x07);
    writeData(display, 0x31);
    writeData(display, 0xC1);
    writeData(display, 0x48);
    writeData(display, 0x08);
    writeData(display, 0x0F);
    writeData(display, 0x0C);
    writeData(display, 0x31);
    writeData(display, 0x36);
    writeData(display, 0x0F);

    // EXIT SLEEP
    writeCommand(display, 0x11);
    usleep(120000); // 120ms

    // TURN ON DISPLAY
    writeCommand(display, 0x29);
}

void ili9341_setRotation(ili9341_t *display, uint8_t rotation)
{
#define MADCTL_MY 0x80
#define MADCTL_MX 0x40
#define MADCTL_MV 0x20
#define MADCTL_ML 0x10
// #define MADCTL_RGB 0x00
#define MADCTL_BGR 0x08 // BGR for ILI9341
#define MADCTL_MH 0x04

    /* Rotation Mapping
    0: 0 degrees (portrait)
    1: 90 degrees (landscape)
    2: 180 degrees (portrait)
    3: 270 degrees (landscape)
    */

    writeCommand(display, 0x36); // Memory Access Control

    switch (rotation)
    {
    case 0:
        writeData(display, MADCTL_MX | MADCTL_BGR); // 1 + 0 + 0 + 0 + 0 + 1 + 0 = 0x48
        display->width = ILI9341_WIDTH;
        display->height = ILI9341_HEIGHT;
        break;
    case 1:
        writeData(display, MADCTL_MV | MADCTL_BGR); // 0 + 0 + 1 + 0 + 0 + 1 + 0 = 0x28
        display->width = ILI9341_HEIGHT;
        display->height = ILI9341_WIDTH;
        break;
    case 2:
        writeData(display, MADCTL_MY | MADCTL_BGR); // 1 + 1 + 0 + 0 + 0 + 1 + 0 = 0xC8
        display->width = ILI9341_WIDTH;
        display->height = ILI9341_HEIGHT;
        break;
    case 3:
        writeData(display, MADCTL_MX | MADCTL_MV | MADCTL_BGR); // 1 + 0 + 1 + 0 + 0 + 1 + 0 = 0xE8
        display->width = ILI9341_HEIGHT;
        display->height = ILI9341_WIDTH;
        break;
    }
} // setRotation

// This Inverts Colour not flip.
void ili9341_invertDisplay(ili9341_t *display, int invert)
{
    writeCommand(display, invert ? 0x21 : 0x20); // 0x21 = Invert On, 0x20 = Invert Off, DINVON and DINVOFF command
}

void ili9341_drawChar(ili9341_t *display, char character, uint8_t x, uint8_t y, uint16_t colour, uint8_t size, uint16_t bg)
{
    uint8_t func_char, i, j;

    func_char = character;

    if (func_char < ' ')
    {
        func_char = 0;
    }
    else
    {
        func_char -= 32; // Adjust to start of font table
    }

    char temp[CHAR_WIDTH];

    for (uint8_t k = 0; k < CHAR_WIDTH; k++)
    {
        temp[k] = font[func_char][k]; // add char to temp array, 0 is space, a is 33, etc.
    }

    // drawing
    ili9341_fill_rect(display, x, y, CHAR_WIDTH * size, CHAR_HEIGHT * size, bg); // background of the text

    // going through pixels of character
    for (j = 0; j < CHAR_WIDTH; j++)
    {
        for (i = 0; i < CHAR_HEIGHT; i++)
        {
            if (temp[j] & (1 << i)) // move 1 to bit position i and check if its turned on
            {
                if (size == 1)
                {
                    drawPixel(display, x + j, y + i, colour);
                }
                else
                {
                    ili9341_fill_rect(display, x + (j * size), y + (i * size), size, size, colour); // one pixel is bigger.
                }
            }
        }
    }
}

void ili9341_drawText(ili9341_t *display, const char *text, uint8_t x, uint8_t y, uint16_t colour, uint8_t size, uint16_t bg)
{
    while (*text) // pointer to current char
    {
        // loop thru all char
        ili9341_drawChar(display, *text++, x, y, colour, size, bg);
        x += CHAR_WIDTH * size;
    }
}