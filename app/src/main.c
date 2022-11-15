//============================================================================
// Name        : main.cpp
// Author      : Iulian Gheorghiu (morgoth@devboard.tech)
// Version     : 1.0
// Copyright   : GNUv2
// Description : Led play
//============================================================================
#include <stdbool.h>
#include "riscv_hal.h"
#include "3d.h"
#include "io.h"

#define CPU_FREQ		(74250000)
#define delay_cycles	2
//#define delay_cycles	(CPU_FREQ / 23)
//#define RTC_PERIOD		(CPU_FREQ / 1000)

//#define LED_IO				(*(PORT_t *) 0x100)  /* Virtual Port */
#define VIDEO				(*(VIDEO_t *) 0x100)  /* Virtual Port */

//#define SET		OUTSET
//#define CLEAR	OUTCLR


volatile unsigned long long STimerCnt = 0;
volatile bool ledState = false;

#ifdef __cplusplus
extern "C" {
#endif

int IRQHandler_1(unsigned int int_nr)
{
	STimerCnt ++;
	return 1;
}
#ifdef __cplusplus
}
#endif

void delay(unsigned long time)
{
	unsigned long long time_to_tick = STimerCnt + time;
	unsigned long long rtc_cnt_int;
	do
	{
		rtc_cnt_int = STimerCnt;
	} while (time_to_tick > rtc_cnt_int);

	/*STimerCnt = time;
	while(STimerCnt)
	{
		STimerCnt--;
	};*/

}


/*void left_to_right(unsigned long times)
{
	LED_IO.CLEAR = 0xFF;
	delay(times);
	LED_IO.SET = 0b00000001;
	delay(times);
	LED_IO.CLEAR = 0b00000001;
	LED_IO.SET = 0b00000010;
	delay(times);
	LED_IO.CLEAR = 0b00000010;
	LED_IO.SET = 0b00000100;
	delay(times);
	LED_IO.CLEAR = 0b00000100;
	LED_IO.SET = 0b00001000;
	delay(times);
	LED_IO.CLEAR = 0b00001000;
	LED_IO.SET = 0b00010000;
	delay(times);
	LED_IO.CLEAR = 0b00010000;
	LED_IO.SET = 0b00100000;
	delay(times);
	LED_IO.CLEAR = 0b00100000;
	LED_IO.SET = 0b01000000;
	delay(times);
	LED_IO.CLEAR = 0b01000000;
	LED_IO.SET = 0b10000000;
	delay(times);
	LED_IO.CLEAR = 0xFF;
	delay(times);
}

void right_to_left(unsigned long times)
{
	LED_IO.CLEAR = 0xFF;
	delay(times);
	LED_IO.SET = 0b10000000;
	delay(times);
	LED_IO.CLEAR = 0b10000000;
	LED_IO.SET = 0b01000000;
	delay(times);
	LED_IO.CLEAR = 0b01000000;
	LED_IO.SET = 0b00100000;
	delay(times);
	LED_IO.CLEAR = 0b00100000;
	LED_IO.SET = 0b00010000;
	delay(times);
	LED_IO.CLEAR = 0b00010000;
	LED_IO.SET = 0b00001000;
	delay(times);
	LED_IO.CLEAR = 0b00001000;
	LED_IO.SET = 0b00000100;
	delay(times);
	LED_IO.CLEAR = 0b00000100;
	LED_IO.SET = 0b00000010;
	delay(times);
	LED_IO.CLEAR = 0b00000010;
	LED_IO.SET = 0b00000001;
	delay(times);
	LED_IO.CLEAR = 0xFF;
	delay(times);
}

void center_to_center(unsigned long times)
{
	LED_IO.CLEAR = 0xFF;
	delay(times);
	LED_IO.SET = 0b10000001;
	delay(times);
	LED_IO.CLEAR = 0b10000001;
	LED_IO.SET = 0b01000010;
	delay(times);
	LED_IO.CLEAR = 0b01000010;
	LED_IO.SET = 0b00100100;
	delay(times);
	LED_IO.CLEAR = 0b00100100;
	LED_IO.SET = 0b00011000;
	delay(times);
	LED_IO.CLEAR = 0b00011000;
	LED_IO.SET = 0b00100100;
	delay(times);
	LED_IO.CLEAR = 0b00100100;
	LED_IO.SET = 0b01000010;
	delay(times);
	LED_IO.CLEAR = 0b01000010;
	LED_IO.SET = 0b10000001;
	delay(times);
	LED_IO.CLEAR = 0xFF;
	delay(times);
}
*/
unsigned char cnt = 0;

_3d_points_i Points_i;
static int16_t angle_x = 0;
static int16_t angle_y = 0;
static int16_t angle_z = 0;

int
main() {
	//LED_IO.DIR = 0xFFFFFFFF;
	//LED_IO.CLEAR = 0xFF;
	box_t box;

	int32_t x[3];
	int32_t y[3];
	int32_t z[3];
	int32_t depth[3];
	int32_t scaleFactor[3];

    Points_i.x = x;
    Points_i.y = y;
    Points_i.z = z;
    Points_i.depth = depth;
    Points_i.scaleFactor = scaleFactor;

    Points_i.x[0] = -200;
    Points_i.y[0] = -200;
    Points_i.z[0] = -200;
    Points_i.x[1] = 200;
    Points_i.y[1] = -200;
    Points_i.z[1] = -200;
    Points_i.x[2] = 0;
    Points_i.y[2] = 200;
    Points_i.z[2] = -200;
    Points_i.pointsNr = 3;// Three points to be processed.
    Points_i.focalLength = 300;
	Points_i.depthScale = 1;

    while(1) {
		lib_3ds_clear(0x00);
		if(angle_y < 360)
			angle_y++;
		else
			angle_y = 0;
    	lib_3ds_draw_triangle_i(NULL, &Points_i, 128, 128, angle_x, angle_y, angle_z, 0xFF);
    	uint8_t shift = 128;
    	uint8_t d = BANK_3_IO.IN;
    	int x = 5;
    	while(shift) {
    		bool fill = false;
    		if(d & shift) {
    			fill = true;
    		}
    		lib_3ds_draw_circle(NULL, x, 192, 3, fill, 0xFF);
    		x += 8;
    		shift = shift >> 1;
    	}
    	shift = 128;
    	d = BANK_2_IO.IN;
    	x = 85;
    	while(shift) {
    		bool fill = false;
    		if(d & shift) {
    			fill = true;
    		}
    		lib_3ds_draw_circle(NULL, x, 192, 3, fill, 0xFF);
    		x += 8;
    		shift = shift >> 1;
    	}
    	shift = 128;
    	d = BANK_1_IO.IN;
    	x = 165;
    	while(shift) {
    		bool fill = false;
    		if(d & shift) {
    			fill = true;
    		}
    		lib_3ds_draw_circle(NULL, x, 192, 3, fill, 0xFF);
    		x += 8;
    		shift = shift >> 1;
    	}
    	shift = 128;
    	d = BANK_0_IO.IN;
    	x = 245;
    	while(shift) {
    		bool fill = false;
    		if(d & shift) {
    			fill = true;
    		}
    		lib_3ds_draw_circle(NULL, x, 192, 3, fill, 0xFF);
    		x += 8;
    		shift = shift >> 1;
    	}
    	//delay(2);
    }
}
