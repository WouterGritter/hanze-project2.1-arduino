/*
 * serial.h
 *
 * Created: 29-10-2020 12:55:59
 *  Author: woute
 */ 
#include <avr/io.h>

#ifndef SERIAL_H_
#define SERIAL_H_


void serial_init();

void serial_putc(unsigned char data);
void serial_puts(char* s);
void serial_putU8(unsigned char val);
void serial_putU16(unsigned int val);

char serial_getc(unsigned int timeout);

#endif /* SERIAL_H_ */