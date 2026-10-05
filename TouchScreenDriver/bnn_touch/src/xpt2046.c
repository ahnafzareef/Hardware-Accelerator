#include "xpt2046.h"
#include "ILI9341_XIL.h"
#include <stdio.h>
#include "sleep.h"
#include <stdlib.h>

void xpt2046_init(xpt2046_t *touch, XSpi *spi, XGpio *gpio, unsigned cs_channel, u32 cs_mask, u32 irq_mask, unsigned irq_channel)
{
    touch->spi = spi;
    touch->gpio = gpio;
    touch->cs_channel = cs_channel;
    touch->cs_mask = cs_mask;
    touch->irq_channel = irq_channel;
    touch->irq_mask = irq_mask;

    // idle high
    XGpio_DiscreteSet(gpio, cs_channel, cs_mask);
}

int xpt2046_isTouched(xpt2046_t *touch)
{
    // check if irq is low or not
    uint32_t irq_value = XGpio_DiscreteRead(touch->gpio, touch->irq_channel);
    return (irq_value & touch->irq_mask) == 0; // returns hi if lo.
}

// ReadAxis gets coordinate.
uint16_t xpt2046_readAxis(xpt2046_t *touch, uint8_t command)
{
    // Pull CS low to select the touch controller.
    XGpio_DiscreteClear(touch->gpio, touch->cs_channel, touch->cs_mask);
    // 12 bit ADC, 8 bit on SPI, so: cmd, empty, empty

    uint32_t total = 0;
    int samples = 16; // sampling and avg

    for (int i = 0; i < samples; i++)
    {
        uint8_t tx[3] = {command, 0x00, 0x00};
        uint8_t rx[3] = {0};
        XSpi_Transfer(touch->spi, tx, rx, 3);
        total += ((rx[1] << 8) | rx[2]) >> 3; // 12 bit
    }
    // the first spi transfer MISO nothing, only at second and third rx does it mixo lo and hi of the 12 bit number
    XGpio_DiscreteSet(touch->gpio, touch->cs_channel, touch->cs_mask); // Pull CS high to deselect the touch controller.
    return total / samples;                                            // average
}

int xpt2046_readPointCalibrated(xpt2046_t *touch, uint16_t *x, uint16_t *y)
{
    uint16_t rawx, rawy;
    if (!xpt2046_readPoint(touch, &rawx, &rawy))
        return 0;

    int px = (rawx - X_MIN) * ILI9341_WIDTH / (X_MAX - X_MIN);
    int py = (Y_MAX - rawy) * ILI9341_HEIGHT / (Y_MAX - Y_MIN);

    if (px < 0)
        px = 0;
    if (px >= ILI9341_WIDTH)
        px = ILI9341_WIDTH - 1;
    if (py < 0)
        py = 0;
    if (py >= ILI9341_HEIGHT)
        py = ILI9341_HEIGHT - 1;

    *x = (uint16_t)px;
    *y = (uint16_t)py;
    return 1;
}

int xpt2046_readPoint(xpt2046_t *touch, uint16_t *x, uint16_t *y)
{
    if (!xpt2046_isTouched(touch))
        return 0;

    *x = xpt2046_readAxis(touch, 0xD0); // x
    *y = xpt2046_readAxis(touch, 0x90); // y

    return 1;
}