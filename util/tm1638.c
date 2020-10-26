/*
 * tm1638.c
 *
 * Created: 26-10-2020 13:01:04
 *  Author: woute
 */ 

#include "tm1638.h"

#include "../settings.h"
#include "functions.h"
#include <avr/io.h>

#define PIN_PB_TM1638_DATA   3 // PB3 (pin 11)
#define PIN_PB_TM1638_CLOCK  4 // PB4 (pin 12)
#define PIN_PB_TM1638_STROBE 5 // PB5 (pin 13)

// shift out value to data
void tm1638_shiftOut(uint8_t val) {
	uint8_t i;
	for (i = 0; i < 8; i++)  {
		pbWrite(PIN_PB_TM1638_CLOCK, LOW); // bit valid on rising edge
		pbWrite(PIN_PB_TM1638_DATA, val & 1 ? HIGH : LOW); // lsb first
		val = val >> 1;
		pbWrite(PIN_PB_TM1638_CLOCK, HIGH);
	}
}

// shift in value from data
uint8_t tm1638_shiftIn() {
	uint8_t value = 0;
	uint8_t i;

	pbMode(PIN_PB_TM1638_DATA, INPUT);
	
	for (i = 0; i < 8; ++i) {
		pbWrite(PIN_PB_TM1638_CLOCK, LOW);   // bit valid on rising edge
		value = value | (pbRead(PIN_PB_TM1638_DATA) << i); // lsb first
		pbWrite(PIN_PB_TM1638_CLOCK, HIGH);
	}
	
	pbMode(PIN_PB_TM1638_DATA, OUTPUT);
	
	return value;
}

void tm1638_sendCommand(uint8_t value) {
	pbWrite(PIN_PB_TM1638_STROBE, LOW);
	tm1638_shiftOut(value);
	pbWrite(PIN_PB_TM1638_STROBE, HIGH);
}

void tm1638_setup() {
	pbMode(PIN_PB_TM1638_DATA, OUTPUT);
	pbMode(PIN_PB_TM1638_CLOCK, OUTPUT);
	pbMode(PIN_PB_TM1638_STROBE, OUTPUT);

	tm1638_sendCommand(0x89);  // activate and set brightness to medium
	
	tm1638_reset();
}

void tm1638_reset() {
	// clear memory - all 16 addresses
	tm1638_sendCommand(0x40); // set auto increment mode
	pbWrite(PIN_PB_TM1638_STROBE, LOW);
	tm1638_shiftOut(0xc0);   // set starting address to 0
	for(uint8_t i = 0; i < 16; i++) {
		tm1638_shiftOut(0x00);
	}
	
	pbWrite(PIN_PB_TM1638_STROBE, HIGH);
	
	// clear internal stuff
	for(auto i = 0; i < 8; i++) tm1638_segments[i] = 0;
	tm1638_leds = 0;
}

int tm1638_readButtons() {
	uint8_t buttons = 0;
	pbWrite(PIN_PB_TM1638_STROBE, LOW);
	tm1638_shiftOut(0x42); // key scan (read buttons)

	for (uint8_t i = 0; i < 4; i++) {
		uint8_t v = tm1638_shiftIn() << i;
		buttons |= v;
	}

	pbWrite(PIN_PB_TM1638_STROBE, HIGH);
	return buttons;
}

void tm1638_update() {
	tm1638_sendCommand(0x40); // auto-increment address
	pbWrite(PIN_PB_TM1638_STROBE, LOW);
	tm1638_shiftOut(0xc0); // set starting address = 0
	
	for(uint8_t i = 0; i < 8; i++) {
		tm1638_shiftOut(tm1638_segments[i]);
		tm1638_shiftOut((tm1638_leds >> (7 - i)) & 1);
	}
	
	pbWrite(PIN_PB_TM1638_STROBE, HIGH);
}

void tm1638_writeNum(long number, int base) {	
	for(uint8_t i = 0; i < 8; i++) {
		uint8_t digit = number % base;
		
		if(number == 0) {
			tm1638_segments[7 - i] = 0;
		}else{
			tm1638_segments[7 - i] = SEG_DIGITS[digit];
		}
		
		number /= base;
	}
}
