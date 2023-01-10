

#include <avr/io.h>

#ifndef CAN_H_
#define CAN_H_

typedef struct{
	uint16_t ID;
	uint8_t length;
	char data[8];
} CAN_message;

#define CANCTRL_ADDRESS 0x0f
#define LOOPBACK_MODE 0x40
#define NORMAL_MODE 0x00
#define TXB1SIDH 0x31
#define TXB1SIDL 0x32
#define TXB1DLC 0x35
#define TXB1Dm   0x36

#define RXB1SIDL 0x72
#define RXB1SIDH 0x71
#define RXB1DLC	0x75
#define RXB1Dm   0x76

#define RXB0SIDL 0x62
#define RXB0SIDH 0x61
#define RXB0DLC	0x65
#define RXB0Dm   0x66

#define CANINTE 0X2B
#define CANINTF 0X2C

void CAN_init();
void CAN_transmission(CAN_message *mes);
CAN_message CAN_reception(uint8_t channel);

#endif /* CAN_H_ */