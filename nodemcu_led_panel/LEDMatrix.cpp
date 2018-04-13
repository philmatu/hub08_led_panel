/**
 * LED Matrix library for https://www.seeedstudio.com/Ultrathin-16x32-RGB-LED-Matrix-Panel-p-1926.html
 * The LED Matrix panel has 32x16 pixels.
 * 
 * Inspired by https://github.com/Seeed-Studio/Ultrathin_LED_Matrix/

 * Revised and adapted for RGB panels by Mathieu Agopian
 */

#include "LEDMatrix.h"
#include "Arduino.h"
#include <stdint.h>
//#include <SPI.h>


LEDMatrix::LEDMatrix(uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint8_t oe, uint8_t r1, uint8_t g1, uint8_t stb, uint8_t clk) {
    this->clk = clk;
    this->r1 = r1;
    this->g1 = g1;
    this->stb = stb;
    this->oe = oe;
    this->a = a;
    this->b = b;
    this->c = c;
    this->d = d;
}

void LEDMatrix::begin(uint8_t *displaybuf, uint16_t width, uint16_t height) {
    this->displaybuf = displaybuf;
    this->width = width;
    this->height = height;

    pinMode(a, OUTPUT);
    pinMode(b, OUTPUT);
    pinMode(c, OUTPUT);
    pinMode(d, OUTPUT);
    pinMode(oe, OUTPUT);
    pinMode(r1, OUTPUT);
    pinMode(g1, OUTPUT);
    pinMode(clk, OUTPUT);
    pinMode(stb, OUTPUT);

    analogWriteFreq(40000000);
    
}

void LEDMatrix::drawPoint(uint16_t x, uint16_t y, uint8_t color) {
    uint16_t index = x + y * width;
    displaybuf[index] = color;
}

void LEDMatrix::scan() {
        static uint8_t row = 0;  // from 0 to 15

    uint8_t *ptr = displaybuf + row * width;
    
    for (uint8_t col = 0; col < width; col++) {
        uint8_t pixel = *ptr;
        //pixel = 7;
        
        digitalWrite(clk, LOW);
        
        digitalWrite(r1, pixel & 1);
        
        digitalWrite(g1, pixel & 2);

        digitalWrite(clk, HIGH);

        ptr++;
    }

    digitalWrite(oe, HIGH);              // disable display

    // select row
    digitalWrite(a, (row & 0x01));
    digitalWrite(b, (row & 0x02));
    digitalWrite(c, (row & 0x04));
    digitalWrite(d, (row & 0x08));
    
    
    // latch data
    digitalWrite(stb, LOW);
    digitalWrite(stb, HIGH);
    digitalWrite(stb, LOW);
     
    analogWrite(oe, 20);              // enable display

    row = (row + 1) & 0x0F;
   
}
