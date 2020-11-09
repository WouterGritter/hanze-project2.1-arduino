// Bron: http://cse537-2011.blogspot.com/2011/02/accessing-internal-eeprom-on-atmega328p.html

#include "eeprom.h"
#include <avr/eeprom.h>
#include <avr/interrupt.h>

void eeprom_putc(unsigned int addr, unsigned char data) {
	cli();
	
	//while(EECR & (1 << EEPE));
	
	EEAR = addr;
	EEDR = data;
	EECR |= (1 << EEMPE);
	EECR |= (1 << EEPE);
	
	sei();
}

unsigned char eeprom_getc(unsigned int addr) {
	cli();
	
	//while(EECR & (1 << EEPE));
	
	EEAR = addr;
	EECR |= (1 << EERE);
	
	unsigned char res = EEDR;
	
	sei();
	
	return res;
}

void eeprom_puti(unsigned int addr, unsigned int data) {
	eeprom_putc(addr + 0, data & 0xFF);
	eeprom_putc(addr + 1, data >> 8);
}

unsigned int eeprom_geti(unsigned int addr) {
	return eeprom_getc(addr + 0) |
			(uint16_t) eeprom_getc(addr + 1) << 8;
}
