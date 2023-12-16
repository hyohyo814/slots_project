/*
 * ATmega328p Timer output and pin outputs (current configuration)
 * OC0A - Arduino 6 - pin 12 - PD6 - STEP_3
 * OC0B - Arduino 5 - pin 11 - PD5
 * OC1A - Arduino 9 - pin 15 - PB1
 * OC1B - Arduino 10 - pin 16 - PB2
 * OC2A - Arduino 11 - pin 17 - PB3
 * OC2B - Arduino 3 - pin 5 - PD3
 *
 * BL - MA2
 * R - MA1
 * GR - MB2
 * WH - MB1
 */

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

#include "segments.h"
#include "steppers.h"

// uncomment to remove err
// #include <avr/iom2560.h>


int main() {
  OUT_DIR = 0xff;
  DDRA = 0xff;
  DDRL = 0xff;

  DDRF &= ~(1 << DDF0) & ~(1 << DDF1) &~(1 << DDF2);

  // test reels on startup
  step_test();

  // initialize digits to bet: 00, win: 000, credits: 000 on startup
  initialize_display_values();
  load_user_display();
  /*
   * Insert code to calibrate initial position of reel
   */


  while (1) {
    update_display();

    step_all();
  }
}
