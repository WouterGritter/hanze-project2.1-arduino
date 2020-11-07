#include <avr/io.h>
#include <stdbool.h>


#ifndef PINS_H_
#define PINS_H_


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


#endif /* PINS_H_ */
