#include "settings.h"

#include <avr/io.h>
#include <stdbool.h>
#include <util/delay.h>

#include "util/millis.h"
#include "util/pins.h"
#include "util/tm1638.h"
#include "util/serial.h"

// Pins
#define PIN_PB_LED_CLOSED   0 // PB0 (pin 8)
#define PIN_PB_LED_CHANGING 1 // PB1 (pin 9)
#define PIN_PB_LED_OPENED   2 // PB2 (pin 10)

#define CLOSED 0
#define OPENED 1

uint8_t state = OPENED;
float temperature = 21.5;
int light = 123;

int main(void);
void initialize();
void start();
void readCommand();
void updateLeds();

int main(void) {
	initialize();
	
	start();
}

void initialize() {
	serial_init();
	millis_init();
	tm1638_init();
	
	pbMode(PIN_PB_LED_CLOSED, OUTPUT);
	pbMode(PIN_PB_LED_CHANGING, OUTPUT);
	pbMode(PIN_PB_LED_OPENED, OUTPUT);
}

unsigned long lastStateSend = 0;

void start() {
	serial_puts("<OK>");
	
	unsigned long num = 0;
	while(1) {
		// Read incoming commands
		readCommand();
		
		if(millis() - lastStateSend > 1000) {
			// Send the current state
			sendState();
			lastStateSend = millis();
		}
		
		// Update the LEDs based on the state
		updateLeds();
	}
}

void readCommand() {
	char c = serial_getc(1);
	if(c == '<') {
		uint8_t bufSize = 32;
		char buf[bufSize];
		
		uint8_t index = 0;
		while(1) {
			c = serial_getc(1000);
			if(c == 0 || c == '>') {
				break;
			}
			
			buf[index++] = c;
		}
		
		if(buf[0] == 'S') {
			// Set the state
			switch(buf[1]) {
				case 'c': // closed
				state = CLOSED;
				break;
				case 'o': // opened
				state = OPENED;
				break;
			}
		}

		// Echo the command!
		serial_puts("<E");
		for(int i = 0; i < index; i++) {
			serial_putc(buf[i]);
		}
		serial_putc('>');
	}
}

void sendState() {
	// STATE
	serial_puts("<S");
	switch(state) {
		case CLOSED:
			serial_putc('c');
			break;
		case OPENED:
			serial_putc('o');
			break;
	}
	serial_putc('>');
	
	// TEMPERATURE
	serial_puts("<T");
	serial_putU16(temperature * 100.0);
	serial_putc('>');
	
	// LIGHT
	serial_puts("<L");
	serial_putU16(light);
	serial_putc('>');
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
//		case CLOSING:
//		case OPENING:
//			changingLED = (millis() % 1000) > 500;
//			break;
	}
	
	pbWrite(PIN_PB_LED_CLOSED, closedLED);
	pbWrite(PIN_PB_LED_CHANGING, changingLED);
	pbWrite(PIN_PB_LED_OPENED, openedLED);
}
