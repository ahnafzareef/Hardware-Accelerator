// XNOR-9 touch demo: draw a digit -> BNN accelerator (AXI4-Lite) -> show the answer
#include "ILI9341_XIL.h"
#include "xpt2046.h"
#include "xparameters.h"
#include "xil_io.h"
#include "xil_printf.h"
#include <sleep.h>
#include <string.h>

// ---------------- BNN accelerator registers ----------------
#define BNN_BASE    XPAR_BNN_AXI_0_BASEADDR   // match your xparameters.h name
#define BNN_CTRL    0x00
#define BNN_STATUS  0x04
#define BNN_RESULT  0x08
#define BNN_IMAGE   0x10

// ---------------- screen layout (240 x 320 portrait) ----------------
#define N       28                       // MNIST grid is 28 x 28
#define CELL    7                        // each grid cell = 7 x 7 pixels -> 196 x 196 canvas
#define GX      22                       // canvas left   ((240 - 196) / 2)
#define GY      26                       // canvas top
#define BTN_Y   232                      // buttons under the canvas
#define BTN_H   40
#define CLR_X   22
#define GO_X    128
#define BTN_W   90

XSpi       Spi;
XGpio      Gpio;
ili9341_t  display;
xpt2046_t  touch;

static u8 grid[N][N];                    // 1 = ink

// ---------------- BNN ----------------
static int bnn_classify(const u32 img[25])
{
    for (int k = 0; k < 25; k++)
        Xil_Out32(BNN_BASE + BNN_IMAGE + 4 * k, img[k]);   // send picture
    Xil_Out32(BNN_BASE + BNN_CTRL, 1);                      // start
    while (!(Xil_In32(BNN_BASE + BNN_STATUS) & 1));         // wait for done
    return Xil_In32(BNN_BASE + BNN_RESULT) & 0xF;           // read digit
}

// Center the drawing in the 28x28 box (MNIST digits are centered), then pack:
// pixel = row*28 + col  ->  word pixel/32, bit pixel%32   (same as export script)
static void center_and_pack(u32 img[25])
{
    int minr = N, maxr = -1, minc = N, maxc = -1;
    memset(img, 0, 25 * sizeof(u32));

    for (int r = 0; r < N; r++)
        for (int c = 0; c < N; c++)
            if (grid[r][c]) {
                if (r < minr) minr = r;
                if (r > maxr) maxr = r;
                if (c < minc) minc = c;
                if (c > maxc) maxc = c;
            }
    if (maxr < 0) return;                                   // nothing drawn

    int offr = (N - (maxr - minr + 1)) / 2 - minr;          // shift to the middle
    int offc = (N - (maxc - minc + 1)) / 2 - minc;

    for (int r = minr; r <= maxr; r++)
        for (int c = minc; c <= maxc; c++)
            if (grid[r][c]) {
                int p = (r + offr) * N + (c + offc);
                img[p >> 5] |= 1u << (p & 31);
            }
}

// ---------------- drawing ----------------
static void draw_cell(int r, int c, uint16_t colour)
{
    ili9341_fill_rect(&display, GX + c * CELL, GY + r * CELL, CELL, CELL, colour);
}

static void show_result(char ch, uint16_t colour)
{
    ili9341_drawChar(&display, ch, 200, 2, colour, 3, ILI9341_BLACK);
}

static void clear_canvas(void)
{
    memset(grid, 0, sizeof(grid));
    ili9341_fill_rect(&display, GX, GY, N * CELL, N * CELL, ILI9341_BLACK);
    show_result('?', ILI9341_DARKGREY);
}

static void draw_ui(void)
{
    ili9341_fill_screen(&display, ILI9341_BLACK);
    ili9341_drawText(&display, "Draw a digit", 4, 6, ILI9341_WHITE, 2, ILI9341_BLACK);

    // grey border around the canvas
    ili9341_fill_rect(&display, GX - 2, GY - 2, N * CELL + 4, N * CELL + 4, ILI9341_DARKGREY);
    clear_canvas();

    ili9341_fill_rect(&display, CLR_X, BTN_Y, BTN_W, BTN_H, ILI9341_RED);
    ili9341_drawText(&display, "CLEAR", CLR_X + 15, BTN_Y + 15, ILI9341_WHITE, 2, ILI9341_RED);
    ili9341_fill_rect(&display, GO_X, BTN_Y, BTN_W, BTN_H, ILI9341_DARKGREEN);
    ili9341_drawText(&display, "GO", GO_X + 33, BTN_Y + 15, ILI9341_WHITE, 2, ILI9341_DARKGREEN);
}

// 2x2 brush so strokes are thick like MNIST digits
static void paint(int r, int c)
{
    for (int dr = 0; dr < 2; dr++)
        for (int dc = 0; dc < 2; dc++) {
            int rr = r + dr, cc = c + dc;
            if (rr < N && cc < N && !grid[rr][cc]) {
                grid[rr][cc] = 1;
                draw_cell(rr, cc, ILI9341_WHITE);
            }
        }
}

static int inside(uint16_t x, uint16_t y, int bx, int by, int bw, int bh)
{
    return x >= bx && x < bx + bw && y >= by && y < by + bh;
}

static void wait_release(void)
{
    while (xpt2046_isTouched(&touch))
        usleep(10000);
}

// ---------------- main ----------------
int main(void)
{
    // SPI (shared by display + touch)
    XSpi_Config *spiCfg = XSpi_LookupConfig(XPAR_AXI_QUAD_SPI_0_BASEADDR);
    XSpi_CfgInitialize(&Spi, spiCfg, spiCfg->BaseAddress);
    XSpi_SetOptions(&Spi, XSP_MASTER_OPTION | XSP_MANUAL_SSELECT_OPTION);
    XSpi_SetSlaveSelect(&Spi, 0x01);
    XSpi_Start(&Spi);
    XSpi_IntrGlobalDisable(&Spi);

    // GPIO (display DC/RST, touch CS, touch IRQ)
    XGpio_Initialize(&Gpio, XPAR_AXI_GPIO_0_BASEADDR);
    XGpio_SetDataDirection(&Gpio, 1, 0x0);

    ili9341_init(&display, &Spi, &Gpio, 0x01, 0x02, 1);
    xpt2046_init(&touch, &Spi, &Gpio, 1, 0x04, 0x01, 2);

    draw_ui();
    xil_printf("XNOR-9 ready\r\n");

    uint16_t px, py;
    u32 img[25];

    while (1) {
        if (xpt2046_readPointCalibrated(&touch, &px, &py)) {

            if (inside(px, py, GX, GY, N * CELL, N * CELL)) {          // drawing
                paint((py - GY) / CELL, (px - GX) / CELL);
            }
            else if (inside(px, py, CLR_X, BTN_Y, BTN_W, BTN_H)) {     // CLEAR
                clear_canvas();
                wait_release();
            }
            else if (inside(px, py, GO_X, BTN_Y, BTN_W, BTN_H)) {      // GO
                center_and_pack(img);
                int digit = bnn_classify(img);
                show_result('0' + digit, ILI9341_GREEN);
                xil_printf("digit = %d\r\n", digit);
                wait_release();
            }
        }
        usleep(2000);
    }
    return 0;
}