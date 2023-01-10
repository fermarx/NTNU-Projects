#include <avr/io.h> //Defines pins, ports, etc.
#include <stdio.h>
#include <stdint.h>

#include "OLED.h"
#include "sram_test.h"
#include "adc.h"
#include "can_driver.h"

#define F_CPU 4915200 // Clock Speed
#include <util/delay.h>

#define BAUD 9600
#define MYUBRR F_CPU/16/BAUD-1

void USART_Transmit( unsigned char data ){
	
	/* Wait for empty transmit buffer */
	while ( !( UCSR0A & (1<<UDRE0)) );
	
	/* Put data into buffer, sends the data */
	UDR0 = data;
}

unsigned char USART_Receive(){
	
	/* Wait for data to be received */
	while ( !(UCSR0A & (1<<RXC0)) );
	
	/* Get and return received data from buffer */
	return UDR0;
}

void USART_Init( unsigned int ubrr ){
	
	/* Set baud rate */
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char)ubrr;
	/* Enable receiver and transmitter */
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	/* Set frame format: 8data, 2stop bit */
	UCSR0C = (1<<URSEL0)|(1<<USBS0)|(3<<UCSZ00);
	/* Enable External memory */
	MCUCR = (1<<SRE);
	/* Enable Memory Bus, Mask out AVR pins*/
	SFIOR = (1<<XMM2) | (1<<XMBK);
}

int main(){
	//volatile char *ext_ram = (char *) 0x1000; // Start address for the SRAM
	USART_Init ( MYUBRR );
	fdevopen(USART_Transmit, USART_Receive);
	
	char* test;
	CAN_init();
	CAN_message prueba;
	prueba.ID = 14;
	
	while (1) {
		
		test = adc_read(0b10);
	
		int len  = 0;
		for (int i = 0; i < strlen(test); i++ ){
			len ++;
			prueba.data[i] = test[i];
		}
		prueba.length = len;
		
		CAN_transmission(&prueba);
		uint8_t status = mcp2515_read_status();
		//printf("%x\n\r",status);
		if(status & 2){
			//CAN_reception(1);
			
		}else if (status & 1){
			//CAN_reception(0);
			
		}
		_delay_ms(500);
	}
	
	
	
	/*oled_init();
	oled_reset();
	oled_print(&prueba.data);*/
	return 0;
	
	
	
}
/*main ex1:
