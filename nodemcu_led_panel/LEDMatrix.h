/**
 * LED Matrix library for https://www.seeedstudio.com/Ultrathin-16x32-RGB-LED-Matrix-Panel-p-1926.html
 * The LED Matrix panel has 32x16 pixels.
 * 
 * Inspired by https://github.com/Seeed-Studio/Ultrathin_LED_Matrix/
 * 
 * Revised and adapted for RGB panels by Mathieu Agopian
 */


#ifndef __LED_MATRIX_H__
#define __LED_MATRIX_H__

#include <stdint.h>

class LEDMatrix {
public:
    LEDMatrix(uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint8_t oe, uint8_t r1, uint8_t g1, uint8_t stb, uint8_t clk);

    /**
     * @param displaybuf    display buffer
     * @param width         panels' width
     * @param height        panel's height
     */
    void begin(uint8_t *displaybuf, uint16_t width, uint16_t height);

    /**
     * draw a point
     * @param x     x
     * @param y     y
     * @param color coded on 3 bits for RGB, eg: 1: Red, 2: Green, 4: Blue, 7: White (Red + Green + Blue)
     */
    void drawPoint(uint16_t x, uint16_t y, uint8_t color);

    /**
     * Scan the whole display buffer and "apply" it to the leds.
     */
    void scan();

private:
    uint8_t clk, r1, g1, stb, oe, a, b, c, d;
    uint8_t *displaybuf;
    uint16_t width;
    uint16_t height;
};

#endif

