#ifndef __SEGMENTS_H__
#define __SEGMENTS_H__

// Direction
#define D_SEG_a DDA0
#define D_SEG_b DDA1
#define D_SEG_c DDA2
#define D_SEG_d DDA3
#define D_SEG_e DDA4
#define D_SEG_f DDA5
#define D_SEG_g DDA6
#define D_SEG_dp DDA7

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

#define DIG_1 PL0 // D49
#define DIG_2 PL1 // D48
#define DIG_3 PL2 // D47
#define DIG_4 PL3 // D46
#define DIG_5 PL4 // D45
#define DIG_6 PL5 // D44
#define DIG_7 PL6 // D43
#define DIG_8 PL7 // D42

// These are BCD values. i.e limited to 0-9.
char bet[2];          // DIG_0, DIG_1
char win[3];          // DIG_2, DIG_3, DIG_4
char credits[3];      // DIG_5, DIG_6, DIG_7

// These can be any decodable values by the segTable[].
char displayBuff[8];

// 7-seg table
//        a
//      f   b
//        g
//      e   c
//        d   dp
//

void update_display();
void load_user_display();

#endif // !__SEGMENTS_H__
