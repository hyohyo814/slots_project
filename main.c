#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

#include "segments.h"
#include "steppers.h"

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
