#include <string.h>
#include <avr/pgmspace.h>
#include "OLED.h"
#include "fonts.h"

/****************** PRIVATE FUNCTIONS ******************/

//Function used to write in the OLED screen
void write_c(uint8_t command) {
	volatile char *ext_oled_command = (char *) 0x1000;//Start address for the OLED command
	ext_oled_command[0] = command;
}

//Function used to read in the OLED screen
void write_d(uint8_t data) {
	volatile char *ext_oled_data = (char *) 0x1200;//Start address for the OLED data
	ext_oled_data[0] = data;
}


/****************** PUBLIC FUNCTIONS ******************/

void oled_init() {
	write_c(0xae); // display off
	write_c(0xa1); //segment remap
	write_c(0xda); //common pads hardware: alternative
	write_c(0x12);
	write_c(0xc8); //common output scan direction:com63~com0
	write_c(0xa8); //multiplex ration mode:63
	write_c(0x3f);
	write_c(0xd5); //display divide ratio/osc. freq. mode
	write_c(0x80);
	write_c(0x81); //contrast control
	write_c(0x50);
	write_c(0xd9); //set pre-charge period
	write_c(0x21);
	write_c(0x20); //Set Memory Addressing Mode
	write_c(0x02);
	write_c(0xdb); //VCOM deselect level mode
	write_c(0x30);
	write_c(0xad); //master configuration
	write_c(0x00);
	write_c(0xa4); //out follows RAM content
	write_c(0xa6); //set normal display
	write_c(0xaf); // display on

	// Page addressing mode
	write_c(0x20);
	write_c(0b10);
}

//With this function we can clear all the data displayed on the screen
void oled_reset(){
	//We setup a loop that deletes all lines on the screen
	for (int i=0; i < 8; i++) {
		oled_clear_line(i);
	}
	oled_home();
}

//This returns the position to the first one on the screen
void oled_home(){
	oled_pos(0,0);
}

//This function goes to the line specified in the parameter
void oled_goto_line(int line){
	int command = 0xB0 + line;
	write_c(command);
}

//This function goes to the column specified in the parameter
void oled_goto_column(int column){
	write_c(0x00 + (column % 16));
	write_c(0x10 + (column / 16));
}

//This function clears the line specified in the parameter
void oled_clear_line(int line){
	oled_goto_line(line);
	oled_goto_column(0);
	for (int i=0; i<128; i++) {
		write_d(0x00);//For cleaning the line we just write nothing there
	}
}

//Goes to the position given in the parameters, e.g. oled_pos(1,2) goes to those coordinates in the screen
void oled_pos(int row, int column){
	oled_goto_line(row);
	oled_goto_column(column);
	//This combines the two previous functions for moving on the screen
}

//This prints a character on the screen, on the current position
void oled_print_char(char c) {
	
	volatile char *ext_ram = (char *) 0x1800; // Start address for the SRAM
	c = c-32;
	for (int i = 0; i < 7; i++) {
		int read = pgm_read_byte(&font8[c][i]);
		write_d(read);
	}
}

//Prints the message passed on the parameter
void oled_print(char* c){
	for (int i = 0; i < strlen(c); i++) {
		oled_print_char(c[i]);
	}
}