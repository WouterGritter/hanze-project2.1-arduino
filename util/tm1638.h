/*
 * tm1638.h
 *
 * Created: 26-10-2020 13:10:45
 *  Author: woute
 */ 


#ifndef TM1638_H_
#define TM1638_H_


#include <stdbool.h>


#define SEG_A    0b00000001
#define SEG_B    0b00000010
#define SEG_C    0b00000100
#define SEG_D    0b00001000
#define SEG_E    0b00010000
#define SEG_F    0b00100000
#define SEG_G    0b01000000
#define SEG_DOT  0b10000000

int tm1638_segments[8];
int tm1638_leds;

static int SEG_DIGITS[] = {
	SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,         // 0
	SEG_B | SEG_C,                                         // 1
	SEG_A | SEG_B | SEG_G | SEG_E | SEG_D,                 // 2
	SEG_A | SEG_B | SEG_G | SEG_C | SEG_D,                 // 3
	SEG_F | SEG_G | SEG_B | SEG_C,                         // 4
	SEG_A | SEG_F | SEG_G | SEG_C | SEG_D,                 // 5
	SEG_A | SEG_F | SEG_E | SEG_D | SEG_C | SEG_G,         // 6
	SEG_A | SEG_B | SEG_C,                                 // 7
	SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G, // 8
	SEG_A | SEG_B | SEG_C | SEG_D | SEG_F | SEG_G,         // 9
	SEG_A | SEG_B | SEG_C | SEG_E | SEG_F | SEG_G,         // A
	SEG_C | SEG_D | SEG_E | SEG_F | SEG_G,                 // b
	SEG_A | SEG_D | SEG_E | SEG_F,                         // C
	SEG_B | SEG_C | SEG_D | SEG_E | SEG_G,                 // d
	SEG_A | SEG_D | SEG_E | SEG_F | SEG_G,                 // E
	SEG_A | SEG_E | SEG_F | SEG_G,                         // F
};

void tm1638_setup();
void tm1638_reset();

int tm1638_readButtons();

void tm1638_update();

void tm1638_writeNum(long number, int base);


#endif /* TM1638_H_ */