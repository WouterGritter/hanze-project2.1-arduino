/*
 * functions.h
 *
 * Created: 26-10-2020 11:09:59
 *  Author: woute
 */ 


#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

#include <avr/io.h>
#include <stdbool.h>

#define OUTPUT 0
#define INPUT 1

void setPBMode(uint8_t pin, uint8_t mode);

void setPB(uint8_t pin, bool state);


#endif /* FUNCTIONS_H_ */