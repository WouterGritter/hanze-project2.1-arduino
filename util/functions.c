/*
 * functions.c
 *
 * Created: 26-10-2020 11:09:59
 *  Author: woute
 */

#include "functions.h"
#include <avr/io.h>
#include <stdbool.h>

void pbMode(uint8_t pin, bool mode) {
	if(mode) {
		DDRB |= (1 << pin);
	}else {
		DDRB &= ~(1 << pin);
	}
}

void pbWrite(uint8_t pin, bool state) {
	if(state) {
		// On
		PORTB |= (1 << pin);
	}else{
		// Off
		PORTB &= ~(1 << pin);
	}
}

bool pbRead(uint8_t pin) {
	return PINB & (1 << pin);
}
