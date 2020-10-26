/*
 * functions.c
 *
 * Created: 26-10-2020 11:09:59
 *  Author: woute
 */

#include "functions.h"
#include <avr/io.h>
#include <stdbool.h>

inline void setPBMode(uint8_t pin, uint8_t mode) {
	switch(mode) {
		case OUTPUT:
			DDRB |= (1 << pin);
			break;
		case INPUT:
			DDRB &= ~(1 << pin);
			break;
	}
}

inline void setPB(uint8_t pin, bool state) {
	if(state) {
		// On
		PORTB |= (1 << pin);
	}else{
		// Off
		PORTB &= ~(1 << pin);
	}
}
