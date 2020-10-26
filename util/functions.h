/*
 * functions.h
 *
 * Created: 26-10-2020 11:09:59
 *  Author: woute
 */ 

#include <avr/io.h>
#include <stdbool.h>


#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_


#define INPUT 0
#define OUTPUT 1

#define LOW 0
#define HIGH 1

void pbMode(uint8_t pin, bool mode);
void pcMode(uint8_t pin, bool mode);
void pdMode(uint8_t pin, bool mode);

void pbWrite(uint8_t pin, bool state);
void pcWrite(uint8_t pin, bool state);
void pdWrite(uint8_t pin, bool state);

bool pbRead(uint8_t pin);
bool pcRead(uint8_t pin);
bool pdRead(uint8_t pin);


#endif /* FUNCTIONS_H_ */
