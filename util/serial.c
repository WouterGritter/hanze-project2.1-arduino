/*
 * serial.c
 *
 * Created: 29-10-2020 12:55:49
 *  Author: woute
 */ 

#include "serial.h"
#include "../settings.h"
#include <avr/io.h>
#include <util/delay.h>

#include "tm1638.h"

// Bron: http://www.rjhcoding.com/avrc-uart.php
void serial_init() {
    // set baudrate in UBRR
    UBRR0L = (uint8_t)(103 & 0xFF);
    UBRR0H = (uint8_t)(103 >> 8);

    // enable the transmitter and receiver
    UCSR0B |= (1 << RXEN0) | (1 << TXEN0);
}

// Bron: http://www.rjhcoding.com/avrc-uart.php
void serial_putc(unsigned char data) {
	// wait for transmit buffer to be empty
	_delay_us(1000);

	// load data into transmit register
	UDR0 = data;
}

// Bron: http://www.rjhcoding.com/avrc-uart.php
void serial_puts(char* s) {
	// transmit character until NULL is reached
	while(*s > 0) serial_putc(*s++);
}

// Bron: http://www.rjhcoding.com/avrc-uart.php
void serial_putU8(unsigned char val) {
	uint8_t dig1 = '0', dig2 = '0';

	// count value in 100s place
	while(val >= 100)
	{
		val -= 100;
		dig1++;
	}

	// count value in 10s place
	while(val >= 10)
	{
		val -= 10;
		dig2++;
	}

	// print first digit (or ignore leading zeros)
	if(dig1 != '0') serial_putc(dig1);

	// print second digit (or ignore leading zeros)
	if((dig1 != '0') || (dig2 != '0')) serial_putc(dig2);

	// print final digit
	serial_putc(val + '0');
}

// Bron: http://www.rjhcoding.com/avrc-uart.php
void serial_putU16(unsigned int val) {
	uint8_t dig1 = '0', dig2 = '0', dig3 = '0', dig4 = '0';

	// count value in 10000s place
	while(val >= 10000)
	{
		val -= 10000;
		dig1++;
	}

	// count value in 1000s place
	while(val >= 1000)
	{
		val -= 1000;
		dig2++;
	}

	// count value in 100s place
	while(val >= 100)
	{
		val -= 100;
		dig3++;
	}

	// count value in 10s place
	while(val >= 10)
	{
		val -= 10;
		dig4++;
	}

	// was previous value printed?
	uint8_t prevPrinted = 0;

	// print first digit (or ignore leading zeros)
	if(dig1 != '0') {serial_putc(dig1); prevPrinted = 1;}

	// print second digit (or ignore leading zeros)
	if(prevPrinted || (dig2 != '0')) {serial_putc(dig2); prevPrinted = 1;}

	// print third digit (or ignore leading zeros)
	if(prevPrinted || (dig3 != '0')) {serial_putc(dig3); prevPrinted = 1;}

	// print third digit (or ignore leading zeros)
	if(prevPrinted || (dig4 != '0')) {serial_putc(dig4); prevPrinted = 1;}

	// print final digit
	serial_putc(val + '0');
}

// Bron: http://www.rjhcoding.com/avrc-uart.php
char serial_getc(unsigned int timeoutCount) {
	unsigned int counter = 0;
	// wait for data
	while(!(UCSR0A & (1 << RXC0))) {
		if(++counter > timeoutCount) {
			return 0;
		}
	}

	// return data
	return UDR0;
}

// Bron: https://stackoverflow.com/questions/7021725/how-to-convert-a-string-to-integer-in-c
int parseInt(char *str, int offset) {
	str += offset;
	
	int result;

	result = 0;
	while ((*str >= '0') && (*str <= '9')) {
		result = (result * 10) + ((*str) - '0');
		str++;
	}
	return result;
}