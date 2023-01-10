
#include "spi_driver.h"

void init_interrupt(){	
	EMCUCR |= 0X01; //ENABLE INT2
	
}

ISR(INT2_vect){ 
	SPCR |= (1 << MSTR);
}


void SPI_Init(){
	DDRB &=~(1<<PB6);
	/* Set MOSI and SCK output, all others input */
	DDRB |= (1<<PB4)|(1<<PB5)|(1<<PB7);
	/* Enable SPI, Master, set clock rate fck/16 */
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);
}


void SPI_write(char cData){
	
	SPDR = cData;
	/* Wait for transmission complete */
	while(!(SPSR & (1<<SPIF)));
}

char SPI_read(){
	
	SPDR = 0xff;

	while(!(SPSR & (1<<SPIF)));
	
	return SPDR;
}