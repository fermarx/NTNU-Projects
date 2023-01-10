
#include <avr/io.h>

#ifndef SPI_H_
#define SPI_H_

void init_interrupt();

void SPI_Init();

void SPI_write(char cData);

char SPI_read();

#endif /* SPI_H_ */