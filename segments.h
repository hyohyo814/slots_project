#ifndef __SEGMENTS_H__
#define __SEGMENTS_H__

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
