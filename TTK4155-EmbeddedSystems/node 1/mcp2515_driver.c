

#include "mcp2515_driver.h"

void mcp2515_init(){
	
	uint8_t value;
	
	SPI_Init();
	
	mcp2515_reset();
	
	
	value = mcp2515_read(0x0E);
	
	if ((value & 0xE0) != 0x80) {
		printf("MCP2515 is NOT in configuration mode after reset!\n");
		return 1;
	}
	
	mcp2515_write( MCP_CNF1, 0x03);
	mcp2515_write( MCP_CNF2, 0b10110001);
	mcp2515_write( MCP_CNF3, 0x05);
	
	mcp2515_bit_modify( MCP_CANCTRL, 0b11100000, MODE_NORMAL);
	
		
	value = mcp2515_read(MCP_CANSTAT);
	
	if ((value & MODE_MASK) != MODE_NORMAL) {
		printf("MCP2515 is NOT in configuration mode after reset!\n");
		return 1;
	}
	
	
}
void mcp2515_reset(){
	PORTB &= ~(1<<CAN_CS); // lowering !CS
	SPI_write(MCP_RESET);
	PORTB |= (1<<CAN_CS); // raising !CS
}

//_________SPI____________
uint8_t mcp2515_read(uint8_t address){
	uint8_t result;
	
	PORTB &= ~(1<<CAN_CS);
	SPI_write(MCP_READ);
	SPI_write(address);
	result = SPI_read();
	PORTB |= (1<<CAN_CS);
	return result;
}

void mcp2515_write(uint8_t address, uint8_t data){
	uint8_t result;
	PORTB &= ~(1<<CAN_CS);
	SPI_write(MCP_WRITE);
	SPI_write(address);
	
	SPI_write(data);
	
	PORTB |= (1<<CAN_CS);
}

void mcp2515_request_to_send(){
	PORTB &= ~(1<<CAN_CS);
	SPI_write(MCP_RTS);
	PORTB |= (1<<CAN_CS);
}

void mcp2515_bit_modify(uint8_t address, uint8_t mask, uint8_t data){
	PORTB &= ~(1<<CAN_CS);
	SPI_write(MCP_BM);
	SPI_write(address);
	SPI_write(mask);
	SPI_write(data);
	PORTB |= (1<<CAN_CS);
}

uint8_t mcp2515_read_status(){
	uint8_t status;
	
	PORTB &= ~(1<<CAN_CS);
	SPI_write(MCP_RS);
	status = SPI_read();
	PORTB |= (1<<CAN_CS);
	return status;
}