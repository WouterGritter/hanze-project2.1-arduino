#include "settings.h"

#include <avr/io.h>
#include <stdbool.h>
#include <util/delay.h>

#include "util/pins.h"
#include "util/tm1638.h"
#include "util/serial.h"
#include "util/AVR_TTC_scheduler.h"
#include "util/distance.h"
#include <avr/eeprom.h>

// Pins
#define PIN_PB_LED_CLOSED   0 // PB0 (pin 8)
#define PIN_PB_LED_CHANGING 1 // PB1 (pin 9)
#define PIN_PB_LED_OPENED   2 // PB2 (pin 10)

float temperature = 0.0;
float light = 0.0;

int distance = 0;
bool opened = false;
bool automatic = false;

float border_temperature = 0.0;
int border_light = 0;
int border_distance_open = 0;
int border_distance_close = 0;

int main(void);
void initialize();
void start();
void saveToEEPROM();
void loadFromEEPROM();
void readCommand();
void readTemperatureSensor();
void readLightSensor();
void readDistanceSensor();
float lerp(float n, float target, float factor);
void readButtons();
void checkAutomaticStateChange();
bool hasReachedState();
void updateLeds();
void updateTM1638();

/* PINOUT
 *
 * A0: light sensor (with 10k resistor)
 * A1: temperature sensor
 *
 * 6:  distance sensor echo
 * 7:  distance sensor trigger
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

// MAIN
int main(void) {
	// -- INITIALIZE -- //
	serial_init();
	tm1638_init();
	distance_init();

	SCH_Init_T1();
	SCH_Start();

	pbMode(PIN_PB_LED_CLOSED, OUTPUT);
	pbMode(PIN_PB_LED_CHANGING, OUTPUT);
	pbMode(PIN_PB_LED_OPENED, OUTPUT);

	// Enable the ADC and set the pre-scaler to max value (128)
	ADCSRA = 0b10000111;
	
	loadFromEEPROM();
	// -- //
	
	
	// -- PROGRAM CODE -- //

	// SCH_Add_Task(function, delay, interval) - interval is in ms
	SCH_Add_Task(updateLeds, 0, 50);
	SCH_Add_Task(updateTM1638, 0, 50);
	SCH_Add_Task(readButtons, 0, 50);
	SCH_Add_Task(readTemperatureSensor, 500, 1000);
	SCH_Add_Task(readLightSensor, 500, 1000);
	SCH_Add_Task(readDistanceSensor, 500, 200);
	SCH_Add_Task(checkAutomaticStateChange, 1000, 30000);

	// Signal that we're ready for serial commands..
	serial_puts("?OK#");

	while(true) {
		// Read incoming commands from UART
		readCommand();

		// Execute tasks
		SCH_Dispatch_Tasks();
	}
	// -- //
}

/*

bool opened = false;
bool automatic = false;

float border_temperature = 25.0; // default
int border_light = 300; // default
int border_distance_open = 50; // default
int border_distance_close = 10; // default
*/
void saveToEEPROM() {
	eeprom_write_byte (0, opened);					// 1 byte
	eeprom_write_byte (1, automatic);				// 1 byte
	eeprom_write_float(2, border_temperature);		// 2 bytes
	eeprom_write_word (4, border_light);			// 2 bytes
	eeprom_write_word (6, border_distance_open);	// 2 bytes
	eeprom_write_word (8, border_distance_close);	// 2 bytes
}

void loadFromEEPROM() {
	opened					= eeprom_read_byte(0);	// 1 byte
	automatic				= eeprom_read_byte(1);	// 1 byte
	border_temperature		= eeprom_read_float(2);	// 2 bytes
	border_light			= eeprom_read_word(4);	// 2 bytes
	border_distance_open	= eeprom_read_word(6);	// 2 bytes
	border_distance_close	= eeprom_read_word(8);	// 2 bytes
	
	// DEFAULTS
	if(border_temperature == 0)
		border_temperature = 25.0;
	
	if(border_light == 0)
		border_light = 300;
	
	if(border_distance_open == 0)
		border_distance_open = 50;
	
	if(border_distance_close == 0)
		border_distance_close = 15;
}

// Reads commands from the UART and responds to them accordingly.
void readCommand() {
	char c = serial_getc(10);
	if(c == '?') { // ? is the start character
		uint8_t bufSize = 8;
		char buf[bufSize];
		
		uint8_t index = 0;
		while(1) {
			c = serial_getc(5000);
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
			bool modified = true;
			switch(identifier) {
				case 'S': // Status
					opened = (buf[2] == 'o');
					automatic = false; // When setting the status manually, automatic mode is turned off!
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
				default:
					modified = false;
					break;
			}
			
			if(modified) {
				saveToEEPROM();
			}
		}
		
		serial_putc('#'); // # is the end character
	}
}

// Reads the temperature sensor.
void readTemperatureSensor() {
	// Configure ADC to be right justified (10 bit), use AVCC as reference, and select ADC1 as ADC input
	ADMUX = 0b01000001;

	// Start an ADC conversion by setting ADSC bit (bit 6)
	ADCSRA = ADCSRA | (1 << ADSC);
	
	// Wait until the ADSC bit has been cleared
	while(ADCSRA & (1 << ADSC));
	
	float reading = ((5.0 / 1023.0) * ADC) / 0.01; // 10mV per degree!
	if(temperature == 0) {
		temperature = reading;
	}else{
		temperature = lerp(temperature, reading, 0.05);
	}
}

// Reads the light sensor.
void readLightSensor() {
	// Configure ADC to be right justified (10 bit), use AVCC as reference, and select ADC0 as ADC input
	ADMUX = 0b01000000;

	// Start an ADC conversion by setting ADSC bit (bit 6)
	ADCSRA = ADCSRA | (1 << ADSC);
	
	// Wait until the ADSC bit has been cleared
	while(ADCSRA & (1 << ADSC));
	
	int reading = ADC;
	if(light == 0) {
		light = reading;
	}else{
		light = lerp(light, reading, 0.15);
	}
}

void readDistanceSensor() {
	int reading = distance_read();
	if(reading != 0) {
		distance = reading;
	}
}

// Lerp function.
inline float lerp(float n, float target, float factor) {
	return n + (target - n) * factor;
}

// Reads the buttons from the tm1638 and responds to them accordingly.
void readButtons() {
	uint8_t buttons = tm1638_readButtons();
	if(buttons & (1 << 0)) {
		// Open!
		opened = true;
		automatic = false;
		
		saveToEEPROM();
	}else if(buttons & (1 << 1)) {
		// Close!
		opened = false;
		automatic = false;
		
		saveToEEPROM();
	}else if(buttons & (1 << 2)) {
		// Enable automatic mode
		automatic = true;
		
		saveToEEPROM();
	}
}

void checkAutomaticStateChange() {
	if(!automatic) {
		return;
	}
	
	bool shouldClose_light = light > border_light;
	bool shouldClose_temperature = temperature > border_temperature;
	
	if(shouldClose_light || shouldClose_temperature) {
		// One of them says it has to be closed!
		opened = false;
	}else{
		// They both say it has to be opened
		opened = true;
	}
}

bool hasReachedState() {
	bool reachedState = false;
	if(opened) {
		reachedState = distance > border_distance_open;
	}else{
		reachedState = distance < border_distance_close;
	}
	
	// The checks about changing above assume that open_dist > close_dist,
	// but this might not always be the case. Flip the changing flag if it's the other way around (open_dist < close_dist)!
	if(border_distance_open < border_distance_close) {
		reachedState = !reachedState;
	}
	
	return reachedState;
}

// Updates the closed, changing and opened led.
int updateLeds_count = 0;
void updateLeds() {
	pbWrite(PIN_PB_LED_OPENED, opened);
	pbWrite(PIN_PB_LED_CLOSED, !opened);
	
	// This function gets called 20 times per second
	pbWrite(PIN_PB_LED_CHANGING, !hasReachedState() && (updateLeds_count % 20 > 10));
	
	updateLeds_count++;
}

// Updates the tm1638 display.
void updateTM1638() {
	// tm1638_segments[x] = SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G;
	
	for(int i = 0; i < 8; i++)
		tm1638_segments[i] = 0;
	
	if(opened) {
		// OPEN
		tm1638_segments[0] = SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F;			// O
		tm1638_segments[1] = SEG_A | SEG_B | SEG_E | SEG_F | SEG_G;					// P
		tm1638_segments[2] = SEG_A | SEG_D | SEG_E | SEG_F | SEG_G;					// E
		tm1638_segments[3] = SEG_A | SEG_B | SEG_C | SEG_E | SEG_F;					// N
	}else{
		// CLOSED
		tm1638_segments[0] = SEG_A | SEG_D | SEG_E | SEG_F;							// C
		tm1638_segments[1] = SEG_D | SEG_E | SEG_F;									// L
		tm1638_segments[2] = SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F;			// O
		tm1638_segments[3] = SEG_A | SEG_C | SEG_D | SEG_F | SEG_G;					// S
		tm1638_segments[4] = SEG_A | SEG_D | SEG_E | SEG_F | SEG_G;					// E
		tm1638_segments[5] = SEG_B | SEG_C | SEG_D | SEG_E | SEG_G;					// d
	}
	
	if(automatic) {
		// A
		tm1638_segments[7] = SEG_A | SEG_B | SEG_C | SEG_E | SEG_F | SEG_G;			// A
	}
	
	int min = border_distance_open < border_distance_close ? border_distance_open : border_distance_close;
	int max = border_distance_open > border_distance_close ? border_distance_open : border_distance_close;
	int bits = (distance - min) / ((max - min) / 8);
	
	if(border_distance_open > border_distance_close) {
		bits = 8 - bits;
	}
	
	tm1638_leds = 0;
	for(auto i = 0; i < 8; i++) {
		if(i < bits) {
			// Set bit
			tm1638_leds |= (1 << (7 - i));
		}
	}
	
	tm1638_update();
}
