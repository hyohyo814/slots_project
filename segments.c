/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2016-2023 Nels D. "Chip" Pearson (aka CmdrZin)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 *
 * Arduino as Display Controller for the ThreeReelSlot Symulator project
 * 14nov23
 *
 * Every 500 ms
 * This demo updates the three sets of displays; Credits, Win, and Bet.
 *
 * PORTA: Segments
 * PORTL: Digits
 * PORTB: Stepper Outputs
 * PORTH: Sensor Inputs
 *
 */
#include "segments.h"
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

// Direction
#define D_SEG_a DDA0
#define D_SEG_b DDA1
#define D_SEG_c DDA2
#define D_SEG_d DDA3
#define D_SEG_e DDA4
#define D_SEG_f DDA5
#define D_SEG_g DDA6
#define D_SEG_dp DDA7

#define SEG_a PA0
#define SEG_b PA1
#define SEG_c PA2
#define SEG_d PA3
#define SEG_e PA4
#define SEG_f PA5
#define SEG_g PA6
#define SEG_dp PA7

// Direction
#define D_DIG_1 DDL0 // D49
#define D_DIG_2 DDL1 // D48
#define D_DIG_3 DDL2 // D47
#define D_DIG_4 DDL3 // D46
#define D_DIG_5 DDL4 // D45
#define D_DIG_6 DDL5 // D44
#define D_DIG_7 DDL6 // D43
#define D_DIG_8 DDL7 // D42

#define DIG_1 PL0 // D49
#define DIG_2 PL1 // D48
#define DIG_3 PL2 // D47
#define DIG_4 PL3 // D46
#define DIG_5 PL4 // D45
#define DIG_6 PL5 // D44
#define DIG_7 PL6 // D43
#define DIG_8 PL7 // D42

void loadUserDisplay();
void updateDisplay();

const unsigned char segTable[] PROGMEM = {
    // dpgfedcba
    0b00111111, //  0
    0b00000110, //  1
    0b01011011, //  2
    0b01001111, //  3
    0b01100110, //  4
    0b01101101, //  5
    0b01111101, //  6
    0b00000111, //  7
    0b01111111, //  8
    0b01100111, //  9
    0b01110111, //  A
    0b01111100, //  B
    0b00111001, //  C
    0b01011110, //  D
    0b01111001, //  E
    0b01110001, //  F

    0b00000000, //  blank
    0b01000000, //  -
    0b01110100, //  o
    0b00010110, //  L
    0b01000100  //  r
};

// Digit Direction Registers table
const unsigned char dirdigTable[] PROGMEM = {
    D_DIG_1, D_DIG_2, D_DIG_3, D_DIG_4, D_DIG_5, D_DIG_6, D_DIG_7, D_DIG_8};

// Digit table
const unsigned char digTable[] PROGMEM = {DIG_1, DIG_2, DIG_3, DIG_4,
                                          DIG_5, DIG_6, DIG_7, DIG_8};

// Update display from the Display Buffer
void update_display() {
  unsigned char val;
  unsigned char segPattern;
  unsigned char digit;

  for (unsigned char i = 0; i < 8; i++) {
    val = displayBuff[i];
    segPattern = pgm_read_word(&segTable[val]); // get segment pattern
    PORTA = segPattern;
    digit = pgm_read_word(&digTable[i]); // get digit pin
    PORTL |= _BV(digit);
    _delay_ms(1);
    PORTL &= ~_BV(digit);
  }
}
// Copy Bet, Credits, & Win into the Display buffer
void load_user_display() {
  displayBuff[0] = bet[0];
  displayBuff[1] = bet[1];
  displayBuff[2] = win[0];
  displayBuff[3] = win[1];
  displayBuff[4] = win[2];
  displayBuff[5] = credits[0];
  displayBuff[6] = credits[1];
  displayBuff[7] = credits[2];
}