
#include <util/delay.h>
#include "can_driver.h"
#include "mcp2515_driver.h"

void CAN_init() {
	//We initialize the can bus for the transmission
	mcp2515_init();
	
	//Enter NORMAL MODE
	mcp2515_bit_modify(CANCTRL_ADDRESS, 0xE0, NORMAL_MODE);
	mcp2515_write(CANINTE,0X01); // Enable interrupts
	//mcp2515_write(CANCTRL_ADDRESS, NORMAL_MODE);
	
	
	
}

void CAN_transmission(CAN_message *mes) {
	//Function used to transmit the message, we need to write in the  registers located in the mcp2515 the different values of the message struct
	int i;
	mcp2515_write(TXB1SIDH, mes->ID >> 3);
	mcp2515_write(TXB1SIDL, (mes->ID & 0x07) << 5 );
	mcp2515_write(TXB1DLC, mes->length);

	for(i=0; i<mes->length; i++)
		mcp2515_write(TXB1Dm+i, mes->data[i]);
	printf("trans: %x, %x, %s\n\r", mes->ID, mes->length, mes->data);
	mcp2515_request_to_send();
}

//All the registers needed are defined in the CAN data-sheet
CAN_message CAN_reception(uint8_t channel) {
	//This is used to receive the message sent by the other node
	CAN_message mes;
	int i;
	//There are two channels, and we need to read the different registers needed
	if (channel == 0) {
		mes.ID = mcp2515_read(RXB0SIDL) >> 5;
		mes.ID += mcp2515_read(RXB0SIDH) << 3;
		
		mes.length = mcp2515_read(RXB0DLC);

		for(i=0; i<mes.length; i++) {
			mes.data[i] = mcp2515_read(RXB0Dm+i);
			printf("%c\n\r", mes.data[i]);
		}
	}
	else if (channel == 1) {
		mes.ID = mcp2515_read(RXB1SIDL) >> 5;
		mes.ID += mcp2515_read(RXB1SIDH) << 3;
		
		mes.length = mcp2515_read(RXB1DLC);

		for(i=0; i<mes.length; i++) {
			mes.data[i] = mcp2515_read(RXB1Dm+i);
			//printf("%c\n\r", mes.data[i]);
		}	
	}
	
	
	printf("recep: %x, %x, %s\n\r", mes.ID, mes.length, mes.data);
		
	return mes;
	
}