#ifndef XPT2046_H
#define XPT2046_H

// Xilinx Driver APIs for SPI and GPIO.
#include "xspi.h"
#include "xgpio.h"
#include <stdint.h>
#include <xil_types.h>

#define X_MIN 290
#define X_MAX 4000
#define Y_MIN 380
#define Y_MAX 4000

typedef struct
{
    XSpi *spi;            // shared SPI bus (same as display)
    XGpio *gpio;          // shared GPIO
    unsigned cs_channel;  // channel for touch CS (1)
    u32 cs_mask;          // touch CS bit (0x04)
    unsigned irq_channel; // channel for IRQ (2)
    u32 irq_mask;         // IRQ bit (0x01 on channel 2)

} xpt2046_t;
// Member Functions
void xpt2046_init(xpt2046_t *touch, XSpi *spi, XGpio *gpio, unsigned cs_channel, u32 cs_mask, u32 irq_mask, unsigned irq_channel);
uint16_t xpt2046_readAxis(xpt2046_t *touch, uint8_t command);
int xpt2046_isTouched(xpt2046_t *touch);
int xpt2046_readPoint(xpt2046_t *touch, uint16_t *x, uint16_t *y);
int xpt2046_readPointCalibrated(xpt2046_t *touch, uint16_t *x, uint16_t *y);
#endif
