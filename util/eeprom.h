
#ifndef EEPROM_H_
#define EEPROM_H_


void eeprom_putc(unsigned int addr, unsigned char data);
unsigned char eeprom_getc(unsigned int addr);

void eeprom_puti(unsigned int addr, unsigned int data);
unsigned int eeprom_geti(unsigned int addr);


#endif /* EEPROM_H_ */