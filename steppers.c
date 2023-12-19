#include <avr/io.h>
#include <util/delay.h>

#include "steppers.h"

void step1() {
  if (flag == 0) {
    flag = 1;
    PORTB &= ~(1 << PORTB4);
  } else {
    flag = 0;
    PORTB |= (1 << PORTB4);
  }
}

void step2() {
  if (flag == 0) {
    flag = 1;
    PORTB &= ~(1 << PORTB5);
  } else {
    flag = 0;
    PORTB |= (1 << PORTB5);
  }
}

void step3() {
  if (flag == 0) {
    flag = 1;
    PORTB &= ~(1 << PORTB6);
  } else {
    flag = 0;
    PORTB |= (1 << PORTB6);
  }
}
