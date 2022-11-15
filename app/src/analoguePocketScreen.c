/*
 * analoguePocketScreen.c
 *
 *  Created on: Nov 2, 2022
 *      Author: Morgoth
 */

#include "analoguePocketScreen.h"

#if __AVR_MEGA__
#include <avr/pgmspace.h>
static const unsigned char ST7735_BIT_MASK_TABLE [] PROGMEM = {
	0b00000001,
	0b00000010,
	0b00000100,
	0b00001000,
	0b00010000,
	0b00100000,
	0b01000000,
	0b10000000
};
#endif
