/*
 * io.h
 *
 *  Created on: Nov 2, 2022
 *      Author: Morgoth
 */

#ifndef SRC_IO_H_
#define SRC_IO_H_

typedef struct VIDEO_s {
	volatile unsigned char PIXEL;
	volatile unsigned char SET_X_H;
	volatile unsigned char SET_X_L;
	volatile unsigned char SET_Y_H;
	volatile unsigned char SET_Y_L;
	volatile unsigned char SET_WIN_START_X_H;
	volatile unsigned char SET_WIN_START_X_L;
	volatile unsigned char SET_WIN_END_X_H;
	volatile unsigned char SET_WIN_END_X_L;
	volatile unsigned char SET_WIN_START_Y_H;
	volatile unsigned char SET_WIN_START_Y_L;
	volatile unsigned char SET_WIN_END_Y_H;
	volatile unsigned char SET_WIN_END_Y_L;
	volatile unsigned char SET_ACTION;
	volatile unsigned char STATUS;
} VIDEO_t;

#define STATUS_BUSY_BIT		(1 << 0)

#define ACTION_IDLE			0
#define ACTION_DRAW_PIXEL	1
#define ACTION_DRAW_PIXELS	2
#define ACTION_CLEAR		3

typedef struct PORT_s {
	volatile unsigned char OUT;  /* I/O Port Output */
	volatile unsigned char OUTSET;  /* I/O Port Output Set */
	volatile unsigned char OUTCLR;  /* I/O Port Output Clear */
	volatile unsigned char DIR;  /* I/O Port direction, 1=Out, 0=In */
	volatile unsigned char IN;  /* I/O Port input */
} PORT_t;

#define VIDEO				(*(VIDEO_t *) 0x100)
#define BANK_0_IO			(*(PORT_t *) 0x110)
#define BANK_1_IO			(*(PORT_t *) 0x118)
#define BANK_2_IO			(*(PORT_t *) 0x120)
#define BANK_3_IO			(*(PORT_t *) 0x128)
#define BANK_TRAN_IO		(*(PORT_t *) 0x130)





#endif /* SRC_IO_H_ */
