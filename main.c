#include <avr/io.h>
#include <stdbool.h>
#include "util/millis.h"
#include "util/functions.h"


#define PIN_PB_LED_CLOSED   0 // PB0
#define PIN_PB_LED_CHANGING 1 // PB1
#define PIN_PB_LED_OPENED   2 // PB2

#define CLOSED 0
#define OPENED 1
#define CLOSING 2
#define OPENING 3

uint8_t state = OPENING;

int main(void)
{
	initialize();
	
	start();
}

void initialize() {
	initializeMillis();
	
	pbMode(PIN_PB_LED_CLOSED, OUTPUT);
	pbMode(PIN_PB_LED_CHANGING, OUTPUT);
	pbMode(PIN_PB_LED_OPENED, OUTPUT);
}

void start() {
	while(1) {
		updateLeds();		
		
		// simulate changing from state:
		// closed > opening > open > closing > .. (repeat)
		int m = millis() % 10000;
		if(m > 7500) {
			state = CLOSED;
		}else if(m > 5000) {
			state = OPENING;
		}else if(m > 2500) {
			state = OPENED;
		}else if(m > 0) {
			state = CLOSING;
		}
	}
}

void updateLeds() {
	bool closedLED = false;
	bool changingLED = false;
	bool openedLED = false;
	
	switch(state) {
		case CLOSED:
			closedLED = true;
			break;
		case OPENED:
			openedLED = true;
			break;
		case CLOSING:
		case OPENING:
			changingLED = (millis() % 1000) > 500;
			break;
	}
	
	pbWrite(PIN_PB_LED_CLOSED, closedLED);
	pbWrite(PIN_PB_LED_CHANGING, changingLED);
	pbWrite(PIN_PB_LED_OPENED, openedLED);
}