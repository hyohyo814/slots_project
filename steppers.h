#ifndef __STEPPERS_H__
#define __STEPPERS_H__

#define OUT_PORT PORTB
#define OUT_DIR DDRB
#define STEP1 PORTB4
#define STEP2 PORTB5
#define STEP3 PORTB6

void step_test();
void step();
void continuous_test();
void continuous_run();

#endif // !__STEPPERS_H__
