/*
    MIT License

    Copyright (c) 2017-2018, Alexey Dynda

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/
/**
 *   Attiny85 PINS
 *             ____
 *   RESET   -|_|  |- 3V
 *   SCL (3) -|    |- (2)
 *   SDA (4) -|    |- (1)
 *   GND     -|____|- (0)
 *
 *   Atmega328 PINS: connect LCD to A4/A5
 */

#include "ssd1306.h"
#include "nano_engine.h"

/*
 * Heart image below is defined directly in flash memory.
 * This reduces SRAM consumption.
 * The image is define from bottom to top (bits), from left to
 * right (bytes).
 */
const PROGMEM uint8_t heartImage[8] =
{
    0B00001110,
    0B00011111,
    0B00111111,
    0B01111110,
    0B01111110,
    0B00111101,
    0B00011001,
    0B00001110
};

/*
 * Define sprite width. The width can be of any size.
 * But sprite height is always assumed to be 8 pixels
 * (number of bits in single byte).
 */
const int spriteWidth = sizeof(heartImage);

/* Lets show 4 hearts on the display */
const int spritesCount = 4;

/* Declare variable that represents our 4 objects */
struct
{
    NanoPoint pos;
    NanoPoint speed;
} objects[ spritesCount ];

/*
 * Each pixel in SSD1306 display takes 1 bit of the memory. So, full resolution
 * of 128x64 LCD display will require 128*64/8 = 1024 bytes of SRAM for the buffer.
 * To let this example to run on Attiny devices (they have 256/512 byte SRAM), we
 * will use small canvas buffer: 32x32 (requires 128 bytes of SRAM), so the example
 * would run even on Attiny45.
 */
const int canvasWidth = 32; // Width must be power of 2, i.e. 16, 32, 64, 128...
const int canvasHeight = 32; // Height must be divided on 8, i.e. 8, 16, 24, 32...
uint8_t canvasData[canvasWidth*(canvasHeight/8)];
/* Create canvas object */
NanoCanvas1 canvas(canvasWidth, canvasHeight, canvasData);

void setup()
{
    /* Initialize and clear display */
    ssd1306_128x64_i2c_init();
//    ssd1306_128x64_spi_init(3, 4, 5);
//    pcd8544_84x48_spi_init(3, 4, 5); // 3 RST, 4 CES, 5 DS
//    ssd1331_96x64_spi_init(3, 4, 5);
//    ssd1351_128x128_spi_init(3, 4, 5);
//    il9163_128x128_spi_init(3, 4, 5);
//    st7735_128x160_spi_init(3, 4, 5);

    ssd1306_fillScreen(0x00);
    /* Create 4 "hearts", and place them at different positions and give different movement direction */
    for(uint8_t i = 0; i < spritesCount; i++)
    {
        objects[i].speed = { .x = (i & 1) ? -1:  1, .y = (i & 2) ? -1:  1 };
        objects[i].pos = { .x = i*4, .y = i*4 + 2 };
    }
    canvas.setMode( CANVAS_MODE_TRANSPARENT );
}


void loop()
{
    delay(40);

    /* Recalculate position and movement direction of all 4 "hearts" */
    for (uint8_t i = 0; i < spritesCount; i++)
    {
        objects[i].pos += objects[i].speed;
        /* If left or right boundary is reached, reverse X direction */
        if ((objects[i].pos.x == (canvasWidth - 8)) || (objects[i].pos.x == 0))
            objects[i].speed.x = -objects[i].speed.x;
        /* Sprite height is always 8 pixels. Reverse Y direction if bottom or top boundary is reached. */
        if ((objects[i].pos.y == (canvasHeight - 8)) || (objects[i].pos.y == 0))
            objects[i].speed.y = -objects[i].speed.y;
    }

    /* Clear canvas surface */
    canvas.clear();
    /* Draw line */
    canvas.drawLine( 0, 0, canvasWidth*2 - 1, canvasHeight-1);
    /* Draw rectangle around our canvas. It will show the range of the canvas on the display */
    canvas.drawRect(0, 0, canvasWidth-1, canvasHeight-1);
    /* Draw all 4 sprites on the canvas */
    for (uint8_t i = 0; i < spritesCount; i++)
    {
        canvas.drawBitmap1( objects[i].pos.x, objects[i].pos.y, 8, 8, heartImage );
    }
    /* Now, draw canvas on the display */
    canvas.blt(48, 0);
}
