#ifndef ADC_H
#define ADC_H

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