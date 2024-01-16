/*
 * This file is made for use with the Arduino IDE
 */

// Segment port definitions

// Direction
#define D_SEG_a DDA0
#define D_SEG_b DDA1
#define D_SEG_c DDA2
#define D_SEG_d DDA3
#define D_SEG_e DDA4
#define D_SEG_f DDA5
#define D_SEG_g DDA6
#define D_SEG_dp DDA7

// Port
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

// Port
#define DIG_1 PL0 // D49
#define DIG_2 PL1 // D48
#define DIG_3 PL2 // D47
#define DIG_4 PL3 // D46
#define DIG_5 PL4 // D45
#define DIG_6 PL5 // D44
#define DIG_7 PL6 // D43
#define DIG_8 PL7 // D42

// Segment allocated digit spaces
#define BET_DIG 2
#define WIN_DIG 3
#define CREDITS_DIG 3

// Stepper port definitions
#define OUT_DIR DDRB

// Enum type for state machine
enum GAME_STATE gameState;
enum GAME_STATE {GS_IDLE, GS_DAZZLE, GS_SPIN};

#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/io.h>
#include <util/atomic.h>
#include <time.h>
#include <util/delay.h>

/*
 * Function declarations
 */

// stepper functions
void step1();
void step2();
void step3();

// segment functions
void serviceDisplay();
void serviceDazzle();
void initialize_display_values();
void load_user_display();
void dazzleDisplay();

// millis functions
void init_millis(unsigned long);
unsigned long millis();

/*
 * Global variable declarations
 */

// Segment arrays

// Dictionary setup in program memory
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

char bet[2];          // DIG_0, DIG_1
char win[3];          // DIG_2, DIG_3, DIG_4
char credits[3];      // DIG_5, DIG_6, DIG_7

// These can be any decodable values by the segTable[].
char displayBuff[8];

// Stepper flag
unsigned char flag;

// Millis
volatile unsigned long timer1_millis;


// Interrupt definition for millis function
ISR(TIMER4_COMPA_vect) { timer1_millis++; }


// Program entry point
int main() {
  OUT_DIR = 0xff;
  DDRA = 0xff;
  DDRL = 0xff;

  DDRF &= ~(1 << DDF0) & ~(1 << DDF1) & ~(1 << DDF2);

  // initialize digits to bet: 00, win: 000, credits: 000 on startup
  initialize_display_values();
  load_user_display();
  /*
   * Insert code to calibrate initial position of reel
   */

  init_millis(16000000UL);

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
  /* Parameters for controlling stepper speed
  unsigned char speed = 4;
  unsigned long speedTime = 0;
  unsigned int count = 0;
  unsigned long countTime = 1000;
  */

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
      if (lastTime < millis()) {
        // 4 ms
        lastTime += 4;
        step1();
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

// This generates a random segment display to replace the standard User
// Bet,Win,Credits display.
void serviceDazzle() {
  static unsigned long dazzleCheck = 0;

  if (dazzleCheck < millis()) {
    dazzleCheck += 100; // new random patterns.
    for (int i = 0; i < 8; ++i) {
      displayBuff[i] = (char)random(256);
    }
  }
}

// This function uses the RAW data in the display Buffer for Segment control.
// segments: dpgfedcba
// NOTE: This could be tuned tighter by using micros().
void dazzleDisplay() {
  char segPattern;
  static char digit = DIG_1; // physical display digit
  static char index = 0;     // vertual display digit
  static unsigned long lastCheck = 0;

  if (lastCheck < millis()) {
    lastCheck += 1; // 2 ms timing
    PORTL &= ~_BV(digit);
    segPattern = displayBuff[index]; // get segment pattern
    PORTA = segPattern;
    digit = pgm_read_word_near(digTable + index); // get digit pin
    PORTL |= _BV(digit);
    if (++index == 8)
      index = 0; // set up for next digit
  }
}

// Update display from the Display Buffer. Call at least once every 1ms.
// NOTE: This could be tuned tighter by using micros().
void serviceDisplay() {
  char val;
  char segPattern;
  static char digit = DIG_1; // physical display digit
  static char index = 0;     // vertual display digit
  static unsigned long lastCheck = 0;

  if (lastCheck < millis()) {
    lastCheck += 1; // 2 ms timing
    PORTL &= ~_BV(digit);
    val = displayBuff[index];
    segPattern = pgm_read_word_near(segTable + val); // get segment pattern
    PORTA = segPattern;
    digit = pgm_read_word_near(digTable + index); // get digit pin
    PORTL |= _BV(digit);
    if (++index == 8) {
      index = 0; // set up for next digit
    }
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

void initialize_display_values() {
  bet[0] = 0;
  bet[1] = 0;
  win[0] = 0;
  win[1] = 0;
  win[2] = 0;
  credits[0] = 0;
  credits[1] = 0;
  credits[2] = 0;
}

void init_millis(unsigned long f_cpu) {
  unsigned long ctc_match_overflow;

  ctc_match_overflow =
      ((f_cpu / 1000) / 8); // when timer1 is this value, 1ms has passed

  // (Set timer to clear when matching ctc_match_overflow) | (Set clock divisor
  // to 8)
  TCCR4B |= (1 << WGM12) | (1 << CS11);

  // high byte first, then low byte
  OCR4AH = (ctc_match_overflow >> 8);
  OCR4AL = ctc_match_overflow;

  // Enable the compare match interrupt
  TIMSK4 |= (1 << OCIE4A);

  // REMEMBER TO ENABLE GLOBAL INTERRUPTS AFTER THIS WITH sei(); !!!
}

unsigned long millis(void) {
  unsigned long millis_return;

  // Ensure this cannot be disrupted
  ATOMIC_BLOCK(ATOMIC_FORCEON) { millis_return = timer1_millis; }
  return millis_return;
}

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
