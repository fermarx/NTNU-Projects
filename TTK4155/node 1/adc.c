#include <stdlib.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/io.h> //Defines pins, ports, etc.

//Defines the struct for the joystick where we will store the data read from the board
struct Joystick{
	uint16_t position_x;
	uint16_t position_y;
};

//Same as the joystick but with the slider located in the board
struct Slider{
	uint16_t slider_right;
	uint16_t slider_left;
};

//This formula is taken from the ADC data-sheet, and it is used to get the frequency between the measures
static float fclock = (2*1*9)/(4915200 /(2*1*(1+1)));
struct Joystick joystick;
struct Slider slider;

static volatile char *adc_joystick = (char *) 0x1400; // Start address for the JOYSTICK
static uint16_t adc_joystick_size = 0x400; //The size of the memory for the ADC

//Initialization for the ADC, we initialize the different variables needed
void adc_init() {
	
	DDRB |= (1 << PB0);
	TCCR0 = (1 << WGM01) | (1 << COM00) | (1 << CS00);
	OCR0 = 1;
	
}

//This function reads the port of the board and depending on the channel, returns the joystick position or the slider
char* adc_read(int channel){

	int chan = channel | 0x80;
	adc_joystick[0] = chan;
	// printf("%x\r\n", chan);
	uint8_t retreived_value = adc_joystick[0];
	//We introduce a delay sufficient enough to get the changes in the positions we are reading
	_delay_ms(10);
	if ((channel == 0b10) | (channel == 0b11)){
		//This means we are reading the channels from the joystick
		if (channel == 0b10){
			//The Y axis, depending on the value we can calculate if its UP, DOWN or in the middle
			
			joystick.position_y = retreived_value;
			if ((retreived_value > 110) & (retreived_value < 210)) return "CENTER";
			else if ((retreived_value >= 230)) return "UP";
			else if ((retreived_value <= 90)) return "DOWN";
			
			//printf("\r\n\r\n %d \r\n\r\n", retreived_value);
		
		}else if (channel == 0b11){
			//Same as before, but with the X axis
			
			joystick.position_x = retreived_value;
			if ((retreived_value > 110) & (retreived_value < 230)) return "CENTER";
			else if ((retreived_value > 230)) return "RIGHT";
			else if ((retreived_value < 110)) return "LEFT";
		}else return "ERROR";

	}else if ((channel == 0b00) | (channel == 0b01)){
		
		//This means we are reading the slider position, which goes from 0 to 255
		if (channel == 0b00) slider.slider_right = retreived_value;
		else if (channel == 0b01) slider.slider_left = retreived_value;

	}else return "ERROR";
	//If there is a mistake in reading just return an error
	
	/*printf("\r\n\r\n %d en slider_left,  %d en slider_right \r\n\r\n", slider.slider_left, slider.slider_right);
	printf("\r\n\r\n %d en X,  %d en Y\r\n\r\n", (joystick.position_x/255)*100, (joystick.position_y/255)*100);*/
	
	_delay_ms(fclock);
}