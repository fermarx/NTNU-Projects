#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "menu.h"
#include "OLED.h"
#include "adc.h"

#define LIST_LEN 6

/*************************** PRIVATE FUNCTIONS ***************************/

void init_list(char* list[]){
	//This function stores the different arrays displayed on the screen
	list[0] = "--MAIN MENU--";
	list[1] = "PLAY?";
	list[2] = "SELECT DIFFICULTY";
	list[3] = "SETTINGS";
	list[4] = "CREDITS";
	list[5] = "THEME";

}

int print_list(int start, char* list[]){
	//Prints the arrays stored in list[]
	int list_len;
	int i;

	//We place the select arrow at the beginning of the menu
	oled_pos(start,0);
	oled_print(">");
	
	if (LIST_LEN < 7) list_len = LIST_LEN; // OLED just have 7
	else list_len = 7;

	for (i = 0; i < list_len; i++){
		oled_pos(i,8);
		oled_print(list[i]);
	}
	
	return i;

}


/*************************** PUBLIC FUNCTIONS ***************************/

void oled_menu_init(){
	//This function is used to display the menu when is started
	char* str_list[LIST_LEN];
	char *direction_center;
	int row_pos = 1;
	//We clean everything that was on the screen before
	oled_reset();
	//We print the menu with the arrow placed at the first row in the beginning
	init_list(str_list);
	row_pos = print_list(row_pos, str_list);
	row_pos = 1;
	while(1){
		//This loop allows us to navigate on the different arrays of the menu using the joystick
		//We first ask for the direction given by the joystick
		char *direction = adc_read(0b10);
		printf("%s",direction);
		//If we are placed below or above the screen we change the cursor to the edges
		if (row_pos > 7) row_pos = 1;
		if (row_pos < 1) row_pos = LIST_LEN-1;
		//We only navigate through the menu if the direction is UP or DOWN
		if (strcmp(direction, "UP")==0 || strcmp(direction, "DOWN")==0) {
			if(strcmp(direction, "UP")==0) {
				if (row_pos > 1){
					oled_pos(row_pos,0);
					//Then erase the old arrow
					oled_print(" ");
					row_pos -= 1;
					//Change the arrow (>) position to the array below the current one
					oled_pos(row_pos, 0);
					oled_print(">");
				}
				//We need a loop to control that we dont scroll several menus simultaneously
				direction_center = direction;
				while(strcmp(direction_center, "CENTER")!=0){
					//While the direction is different than CENTER the program is stuck here
					direction_center = adc_read(0b10);
					printf("%s\n", direction_center);
				}
			}
			else if (strcmp(direction, "DOWN")==0) {
				//The DOWN direction has the same philosophy as the UP one
				if (row_pos < 7){
					oled_pos(row_pos,0);
					oled_print(" ");
					row_pos += 1;
					oled_pos(row_pos, 0);
					oled_print(">");
				}
				direction_center = direction;
				while(strcmp(direction_center, "CENTER")!=0){
					direction_center = adc_read(0b10);
					printf("%s\n", direction_center);
				}
			}
			
		}
		
	}
}
