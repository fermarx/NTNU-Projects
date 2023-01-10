
#include "spi_driver.h"
#include "mcp2515.h"
#ifndef MCP2515_H_
#define MCP2515_H_


#define MCP_RESET 0xC0
#define MCP_READ 0x03
#define MCP_WRITE 0x02
#define MCP_RTS 0x81
#define MCP_BM 0x05
#define MCP_RS 0xA0
#define MCP_CNF3		0x28
#define MCP_CNF2		0x29
#define MCP_CNF1		0x2A


#define CAN_CS PB4


void mcp2515_init();
void mcp2515_reset();

//_________SPI____________
uint8_t mcp2515_read(uint8_t address);
void mcp2515_write(uint8_t address, uint8_t data);
void mcp2515_request_to_send();
void mcp2515_bit_modify(uint8_t address, uint8_t mask, uint8_t data);
uint8_t mcp2515_read_status();



#endif /* MCP2515_H_ */