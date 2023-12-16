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
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
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
 * rev: 14dec23   Make services and State Machine.
 *
 * One Digit every 2 ms
 * This demo updates the three sets of displays; Credits, Win, and Bet.
 *
 * PORTA: Segments
 * PORTL: Digits
 * PORTB: Stepper Outputs
 * PORTH: Sensor Inputs
 *
 */
#include <avr/pgmspace.h>

#define SEG_a 22  // PA0
#define SEG_b 23  // PA1
#define SEG_c 24  // PA2
#define SEG_d 25  // PA3
#define SEG_e 26  // PA4
#define SEG_f 27  // PA5
#define SEG_g 28  // PA6
#define SEG_dp 29  // PA7

#define DIG_1 49
#define DIG_2 48
#define DIG_3 47
#define DIG_4 46
#define DIG_5 45
#define DIG_6 44
#define DIG_7 43
#define DIG_8 42

enum GAME_STATE {GS_IDLE, GS_DAZZLE, GS_SPIN};

// These are BCD values. i.e limited to 0-9.
char bet[2];          // DIG_0, DIG_1
char win[3];          // DIG_2, DIG_3, DIG_4
char credits[3];      // DIG_5, DIG_6, DIG_7

GAME_STATE gameState;

// These can be any decodable values by the segTable[].
char displayBuff[8];

// 7-seg table
//        a
//      f   b
//        g
//      e   c
//        d   dp
//
const byte segTable[] PROGMEM = {
// dpgfedcba    0x00 - 0x0F
	0b00111111,	//  0
	0b00000110,	//  1
	0b01011011,	//  2
	0b01001111,	//  3
	0b01100110,	//  4
	0b01101101,	//  5
	0b01111101,	//  6
	0b00000111,	//  7
	0b01111111,	//  8
	0b01100111,	//  9
	0b01110111,	//  A
	0b01111100,	//  B
	0b00111001,	//  C
	0b01011110,	//  D
	0b01111001,	//  E
	0b01110001,	//  F

// dpgfedcba with dp    0x10 - 0x1F
	0b10111111,	//  0
	0b10000110,	//  1
	0b11011011,	//  2
	0b11001111,	//  3
	0b11100110,	//  4
	0b11101101,	//  5
	0b11111101,	//  6
	0b10000111,	//  7
	0b11111111,	//  8
	0b11100111,	//  9
	0b11110111,	//  A
	0b11111100,	//  B
	0b10111001,	//  C
	0b11011110,	//  D
	0b11111001,	//  E
	0b11110001,	//  F

// Special     0x20 - 0x2F
	0b00000000,	//  blank
	0b01000000,	//  -
	0b01110100,	//  o
	0b00010110,	//  L
	0b01000100	//  r
};

// Digit table
const byte digTable[] PROGMEM {
  DIG_1, DIG_2, DIG_3, DIG_4, DIG_5, DIG_6, DIG_7, DIG_8
};

void setup() {
  // set up I/O pins
  pinMode(SEG_a, OUTPUT);
  pinMode(SEG_b, OUTPUT);
  pinMode(SEG_c, OUTPUT);
  pinMode(SEG_d, OUTPUT);
  pinMode(SEG_e, OUTPUT);
  pinMode(SEG_f, OUTPUT);
  pinMode(SEG_g, OUTPUT);
  pinMode(SEG_dp, OUTPUT);

  for( int i=0; i<8; i++ ) {
    pinMode(pgm_read_word_near(digTable + i), OUTPUT);
  }

  // Initialize display values
  bet[0] = 0x01;
  bet[1] = 0x05;
  win[0] = 0x03;
  win[1] = 0x02;
  win[2] = 0x08;
  credits[0] = 0x07;
  credits[1] = 0x04;
  credits[2]= 0x05;

  gameState = GS_IDLE;

  loadUserDisplay();
}

void loop() {
  static unsigned long lastTime = 5000;

  switch(gameState) {
    case GS_IDLE:
      serviceDisplay();
      if(lastTime < millis()) {
        lastTime += 2000;             // set Dazzle time
        gameState = GS_DAZZLE;
      }
      break;

    case GS_DAZZLE:
      dazzleDisplay();
      serviceDazzle();
      if(lastTime < millis()) {
        lastTime += 5000;             // set IDLE time
        gameState = GS_IDLE;
        loadUserDisplay();            // reload User data into display Buffer
      }
      break;

    case GS_SPIN:
      break;

    default:
      gameState = GS_IDLE;
      break;
  }

  // Game State Machine - Controls EVERYTHING!
}


// This generates a random segment display to replace the standard User Bet,Win,Credits display.
void serviceDazzle() {
  static unsigned long dazzleCheck = 0;

  if( dazzleCheck < millis() ) {
    dazzleCheck += 100;                     // new random patterns.
    for(int i=0; i<8; ++i) {
      displayBuff[i] = (char)random(256);
    }
  }
}

// This function uses the RAW data in the display Buffer for Segment control.
// segments: dpgfedcba
// NOTE: This could be tuned tighter by using micros().
void dazzleDisplay() {
  char val;
  char segPattern;
  static char digit = DIG_1;                            // physical display digit
  static char index = 0;                                // vertual display digit
  static unsigned long lastCheck = 0;

  if( lastCheck < millis() ) {
    lastCheck += 1;                                     // 2 ms timing
    digitalWrite(digit,LOW);
    segPattern = displayBuff[index];                    // get segment pattern
    PORTA = segPattern;
    digit = pgm_read_word_near(digTable + index);       // get digit pin
    digitalWrite(digit,HIGH);
    if(++index == 8) index = 0;                         // set up for next digit
  }
}

// Update display from the Display Buffer. Call at least once every 1ms.
// NOTE: This could be tuned tighter by using micros().
void serviceDisplay() {
  char val;
  char segPattern;
  static char digit = DIG_1;                            // physical display digit
  static char index = 0;                                // vertual display digit
  static unsigned long lastCheck = 0;

  if( lastCheck < millis() ) {
    lastCheck += 1;               // 2 ms timing
    digitalWrite(digit,LOW);
    val = displayBuff[index];
    segPattern = pgm_read_word_near(segTable + val);    // get segment pattern
    PORTA = segPattern;
    digit = pgm_read_word_near(digTable + index);       // get digit pin
    digitalWrite(digit,HIGH);
    if(++index == 8) index = 0;                         // set up for next digit
  }
}
// Copy Bet, Credits, & Win into the Display buffer
void loadUserDisplay() {
  displayBuff[0] = bet[0];
  displayBuff[1] = bet[1];
  displayBuff[2] = win[0];
  displayBuff[3] = win[1];
  displayBuff[4] = win[2];
  displayBuff[5] = credits[0];
  displayBuff[6] = credits[1];
  displayBuff[7] = credits[2];
}
