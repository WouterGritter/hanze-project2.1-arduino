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

float temperature = 0.0;
int light = 1;
int distance = 1;
bool opened = false;
bool automatic = false;

float border_temperature = 18.0; // default
int border_light = 500; // default
int border_distance_open = 100; // default
int border_distance_close = 20; // default


int main(void);
void initialize();
void start();
void readCommand();
void updateLeds();

/* PINOUT
 *
 * A0: light sensor (with 10k resistor)
 * A1: temperature sensor
 *
 * 8:  closed (red) LED
 * 9:  changing (orange) LED
 * 10: opened (green) LED
 *
 * 11: tm1638 data (DIO)
 * 12: tm1638 clock (CLK)
 * 13: tm1638 strobe (STB)
 *
 */

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
	
	// Enable the ADC and set the pre-scaler to max value (128)
	ADCSRA = 0b10000111;
}

void start() {
	serial_puts("?OK#");
	
	unsigned long num = 0;
	while(1) {
		uint8_t buttons = tm1638_readButtons();
		if(buttons & (1 << 0)) {
			// Open!
			opened = true;
		}else if(buttons & (1 << 1)) {
			// Close!
			opened = false;
		}
		
		// Read sensors
		readTemperatureSensor();
		readLightSensor();
		
		// Read incoming commands
		readCommand();
		
		// Update the LEDs based on the state
		updateLeds();
	}
}

void readCommand() {
	char c = serial_getc(1);
	if(c == '?') { // ? is the start character
		uint8_t bufSize = 8;
		char buf[bufSize];
		
		uint8_t index = 0;
		while(1) {
			c = serial_getc(500);
			if(c == 0 || c == '#') { // # is the end character
				break;
			}

			buf[index++] = c;
			if(index == bufSize) {
				break;
			}
		}
		
		char type = buf[0];
		char identifier = buf[1];
		
		serial_putc('?'); // ? is the start character
		serial_putc(identifier); // Echo back the identifier
		
		if(type == 'G') { // Getter
			switch(identifier) {
				case 'T': // Temperature
					serial_putU16(temperature * 10.0);
					break;
				case 'L': // Light
					serial_putU16(light);
					break;
				case 'D': // Distance
					serial_putU16(distance);
					break;
				case 'S': // Status
					serial_putc(opened ? 'o' : 'c');
					break;
				case 'A': // Automatic mode
					serial_putc(automatic ? 'a' : 'm');
					break;
				case 't': // Temperature border
					serial_putU16(border_temperature * 10.0);
					break;
				case 'l': // Light border
					serial_putU16(border_light);
					break;
				case 'o': // Open distance border
					serial_putU16(border_distance_open);
					break;
				case 'c': // Closed distance border
					serial_putU16(border_distance_close);
					break;
				default: // UNKNOWN
					serial_puts("UNKNOWN");
					break;
			}
		}else if(type == 'S') { // Setter
			switch(identifier) {
				case 'S': // Status
					opened = (buf[2] == 'o');
					break;
				case 'A': // Automatic mode
					automatic = (buf[2] == 'a');
					break;
				case 't': // Temperature border
					border_temperature = parseInt(buf, 2) / 10.0;
					break;
				case 'l': // Light border
					border_light = parseInt(buf, 2);
					break;
				case 'o': // Open distance border
					border_distance_open = parseInt(buf, 2);
					break;
				case 'c': // Closed distance border
					border_distance_close = parseInt(buf, 2);
					break;
			}
		}
		
		serial_putc('#'); // # is the end character
	}
}

void readLightSensor() {
	// Configure ADC to be right justified (10 bit), use AVCC as reference, and select ADC0 as ADC input
	ADMUX = 0b01000000;

	// Start an ADC conversion by setting ADSC bit (bit 6)
	ADCSRA = ADCSRA | (1 << ADSC);
			
	// Wait until the ADSC bit has been cleared
	while(ADCSRA & (1 << ADSC));
	
	light = ADC;
}

void readTemperatureSensor() {
	// Configure ADC to be right justified (10 bit), use AVCC as reference, and select ADC1 as ADC input
	ADMUX = 0b01000001;

	// Start an ADC conversion by setting ADSC bit (bit 6)
	ADCSRA = ADCSRA | (1 << ADSC);
	
	// Wait until the ADSC bit has been cleared
	while(ADCSRA & (1 << ADSC));
	
	temperature = ((5.0 / 1023.0) * ADC) / 0.01; // 10mV per degree!
}

void updateLeds() {
	bool closedLED = false;
	bool changingLED = false;
	bool openedLED = false;
	
	// TODO set changing LED
	// changingLED = (millis() % 1000) > 500;
	if(opened) {
		openedLED = true;
	}else {
		closedLED = true;
	}
	
	pbWrite(PIN_PB_LED_CLOSED, closedLED);
	pbWrite(PIN_PB_LED_CHANGING, changingLED);
	pbWrite(PIN_PB_LED_OPENED, openedLED);
}
