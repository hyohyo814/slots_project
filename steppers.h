#ifndef __STEPPERS_H__
#define __STEPPERS_H__

#define OUT_PORT PORTB
#define OUT_DIR DDRB
#define STEP1 PORTB4
#define STEP2 PORTB5
#define STEP3 PORTB6

unsigned char flag;

void step1();
void step2();
void step3();

#endif // !__STEPPERS_H__
