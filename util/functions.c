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

void pcMode(uint8_t pin, bool mode) {
	if(mode) {
		DDRC |= (1 << pin);
	}else {
		DDRC &= ~(1 << pin);
	}
}

void pdMode(uint8_t pin, bool mode) {
	if(mode) {
		DDRD |= (1 << pin);
	}else {
		DDRD &= ~(1 << pin);
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

void pcWrite(uint8_t pin, bool state) {
	if(state) {
		// On
		PORTC |= (1 << pin);
	}else{
		// Off
		PORTC &= ~(1 << pin);
	}
}

void pdWrite(uint8_t pin, bool state) {
	if(state) {
		// On
		PORTD |= (1 << pin);
	}else{
		// Off
		PORTD &= ~(1 << pin);
	}
}


bool pbRead(uint8_t pin) {
	return PINB & (1 << pin);
}

bool pcRead(uint8_t pin) {
	return PINC & (1 << pin);
}

bool pdRead(uint8_t pin) {
	return PIND & (1 << pin);
}
