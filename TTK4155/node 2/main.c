/*
 * GccApplication3.c
 *
 * Created: 10.11.2021 10:38:50
 * Author : alvaroho
 */ 



// #include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



#include "sam.h"
#include "can_controller.h"
#include "can_interrupt.h"
#include "uart.h"
#include "adc.h"


int main(void){
	SystemInit();
	configure_uart();
	
	
	WDT->WDT_MR = WDT_MR_WDDIS;
	
	can_init_def_tx_rx_mb(0x00290165); //standard baud rate
	
	
	while(1){
		
	}
}
