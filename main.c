/*
 * set PWM output on D10-D12
 *
 * OC2A - PORTB4 - D10
 * OC1A - PORTB5 - D11
 * OC1B - PORTB6 - D12
 *
 *
 */

enum GAME_STATE gameState;

#include <avr/interrupt.h>
#include <avr/io.h>
#include <time.h>
#include <util/delay.h>

#include "millis.h"
#include "segments.h"
#include "steppers.h"

int main() {
  OUT_DIR = 0xff;
  DDRA = 0xff;
  DDRL = 0xff;

  DDRF &= ~(1 << DDF0) & ~(1 << DDF1) & ~(1 << DDF2);

  // Fast PWM with OCRnA Top
  /*
  TCCR1A = (1 << COM1A0) | (1 << COM1B1) | (1 << WGM21) | (1 << WGM20);
  TCCR1B = (1 << WGM22) | (1 << CS22);
  */

  /*
  TCCR1A = (1 << COM1A0) | (1 << COM1B1) | (1 << WGM20);
  TCCR1B = (1 << WGM22) | (1 << CS22);
  */

  /*
  TCCR2A = (1 << COM2A0) | (1 << COM2B1) | (1 << WGM21) | (1 << WGM20);
  TCCR2B = (1 << WGM22) | (1 << CS22);
  */

  // test reels on startup

  // initialize digits to bet: 00, win: 000, credits: 000 on startup
  initialize_display_values();
  load_user_display();
  /*
   * Insert code to calibrate initial position of reel
   */

  init_millis(16000000UL);

  // initialize prev_time
  unsigned long prev_time = millis();

  /*
  OCR2A = 50;
  OCR1A = 30;
  OCR1B = 30;
  */

  sei();

  bet[0] = 0x01;
  bet[1] = 0x02;
  credits[0] = 0x01;
  credits[1] = 0x02;
  credits[2] = 0x03;
  win[0] = 0x01;
  win[1] = 0x02;
  win[2] = 0x03;

  gameState = GS_SPIN;
  unsigned char flag = 0;
  unsigned char speed = 4;
  unsigned long speedTime = 0;
  unsigned int count = 0;
  unsigned long countTime = 1000;

  while (1) {
    static unsigned long lastTime = 1000;

    switch (gameState) {
    case GS_IDLE:
      serviceDisplay();
      if (lastTime < millis()) {
        lastTime += 2000; // set Dazzle time
        gameState = GS_DAZZLE;
      }
      break;

    case GS_DAZZLE:
      dazzleDisplay();
      serviceDazzle();
      if (lastTime < millis()) {
        lastTime += 5000; // set IDLE time
        gameState = GS_IDLE;
        load_user_display(); // reload User data into display Buffer
      }
      break;

    case GS_SPIN:
      serviceDisplay();
      dazzleDisplay();
      if (lastTime < millis()) {
        lastTime += 4;
        if (flag == 0) {
          flag = 1;
          PORTB &= ~(1 << PORTB4);
        } else {
          flag = 0;
          PORTB |= (1 << PORTB4);
        }
      }
      break;

    default:
      gameState = GS_IDLE;
      break;
    }

    /*
     * state machine goes here
     *
     * display refresh - 1 ms
     *
     *
     * STATES:
     * - IDLE
     * - DAZZLe
     * - SPIN
     * - DEFAULT
     */
  }
}
