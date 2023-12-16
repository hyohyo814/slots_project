#include <avr/io.h>
#include <util/delay.h>

#include "steppers.h"

void step_test() {
  for (unsigned int i = 0; i < 200; i++) {
    OUT_PORT &= ~(1 << STEP1);
    _delay_us(10);
    OUT_PORT |= (1 << STEP1);
    _delay_us(8000);
  }
  for (unsigned int i = 0; i < 200; i++) {
    OUT_PORT &= ~(1 << STEP2);
    _delay_us(10);
    OUT_PORT |= (1 << STEP2);
    _delay_us(8000);
  }
  for (unsigned int i = 0; i < 200; i++) {
    OUT_PORT &= ~(1 << STEP3);
    _delay_us(10);
    OUT_PORT |= (1 << STEP3);
    _delay_us(8000);
  }
}

void step_all() {
  OUT_PORT &= ~(1 << STEP1) & ~(1 << STEP2) & ~(1 << STEP3);
  _delay_us(10);
  OUT_PORT |= (1 << STEP1) | (1 << STEP2) | (1 << STEP3);
  _delay_us(10);
}

void step1(unsigned int delay) {
  OUT_PORT &= ~(1 << STEP1);
  _delay_us(10);
  OUT_PORT |= (1 << STEP1);
  _delay_us(delay);
}

void step2(unsigned int delay) {
  OUT_PORT &= ~(1 << STEP2);
  _delay_us(10);
  OUT_PORT |= (1 << STEP2);
  _delay_us(delay);
}

void step3(unsigned int delay) {
  OUT_PORT &= ~(1 << STEP3);
  _delay_us(10);
  OUT_PORT |= (1 << STEP3);
  _delay_us(delay);
}
