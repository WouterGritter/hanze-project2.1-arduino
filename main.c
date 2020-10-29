#include "settings.h"

#include <avr/io.h>
#include <stdbool.h>
#include <util/delay.h>
#include "util/millis.h"
#include "util/pins.h"

#include "util/tm1638.h"

// Pins
#define PIN_PB_LED_CLOSED   0 // PB0 (pin 8)
#define PIN_PB_LED_CHANGING 1 // PB1 (pin 9)
#define PIN_PB_LED_OPENED   2 // PB2 (pin 10)

#define CLOSED 0
#define OPENED 1
#define CLOSING 2
#define OPENING 3

uint8_t state = OPENING;

int main(void);
void initialize();
void start();
void updateLeds();

int main(void)
{
	initialize();
	
	start();
}

void initialize() {
	initializeMillis();
	tm1638_setup();
	
	pbMode(PIN_PB_LED_CLOSED, OUTPUT);
	pbMode(PIN_PB_LED_CHANGING, OUTPUT);
	pbMode(PIN_PB_LED_OPENED, OUTPUT);
}

void start() {
	unsigned long num = 0;
	while(1) {
		// -- TEST DISPLAY --
		tm1638_leds = num;
		tm1638_writeNum(num, 10);
		
		num++;
		
		tm1638_update();
		// ----
		
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
		
		// Update the leds based on the state
		updateLeds();
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
