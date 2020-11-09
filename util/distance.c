#define PIN_PD_TRIGGER 7
#define PIN_PD_ECHO 6

#include "../settings.h"
#include <util/delay.h>
#include <avr/interrupt.h>

#include "distance.h"
#include "pins.h"

void distance_init() {
	pdMode(PIN_PD_TRIGGER, OUTPUT);
	pdMode(PIN_PD_ECHO, INPUT);
}

unsigned int distance_read() {
	cli();
	
	PORTD |=  (1 << PIN_PD_TRIGGER); // On
	_delay_us(10);
	PORTD &= ~(1 << PIN_PD_TRIGGER); // Off
	
	// Wait for the trigger to start..
	unsigned int counter = 0;
	while(!(PIND & (1 << PIN_PD_ECHO))) {
		if(++counter > 1000) { // From testing this value never gets above 260
			break;
		}
	}
	
	counter = 0;
	while((PIND & (1 << PIN_PD_ECHO))) {
		if(++counter > 60000) {
			counter = 0;
			break;
		}
	}
	
	sei();
	
	return counter / 31;
}