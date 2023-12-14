#include <avr/io.h>
#include <util/delay.h>

#include "steppers.h"


void step() {
  OUT_PORT &= ~(1 << STEP1) & ~(1 << STEP2) & ~(1 << STEP3);
  _delay_us(1);
  OUT_PORT |= (1 << STEP1) | (1 << STEP2) | (1 << STEP3);
  _delay_us(1);
}
