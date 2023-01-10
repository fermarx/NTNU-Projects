#ifndef ADC_H
#define ADC_H

//We just need an enum for the different outputs the adc_read can return
typedef enum {
	UP,
	DOWN,
	CENTER,
	RIGHT,
	LEFT,
	ERROR
}Direction;

void adc_init();
int adc_read(int channel);

#endif