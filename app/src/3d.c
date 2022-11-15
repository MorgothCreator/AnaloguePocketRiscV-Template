/*
 * 3D library file for arduFPGA design.
 * 
 * Copyright (C) 2020  Iulian Gheorghiu (morgoth@devboard.tech)
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */


#include <math.h>
#include "3d.h"
#include "io.h"

#define USE_FAST_DRAW_H_LINE

void lib_3ds_draw_pixel(box_t *box, int16_t x, int16_t y, uint8_t color) {
	/* Check if outside the screen */
	if(x < 0 || y < 0 || x > 399 || y > 359)
		return;
	/* Check if outside the window */
	if(box) {
		if(x < box->x_min ||
		x >= box->x_max ||
		y < box->y_min ||
		y >= box->y_max)
			return;
	}
	while(VIDEO.STATUS & STATUS_BUSY_BIT);
	VIDEO.SET_ACTION = ACTION_DRAW_PIXEL;
	VIDEO.SET_X_H = x >> 8;
	VIDEO.SET_X_L = x;
	VIDEO.SET_Y_H = y >> 8;
	VIDEO.SET_Y_L = y;
	VIDEO.PIXEL = color;
}
//#######################################################################################
void lib_3ds_clear(uint8_t color) {
	while(VIDEO.STATUS & STATUS_BUSY_BIT);
	VIDEO.SET_ACTION = ACTION_CLEAR;
	VIDEO.PIXEL = color;
}
//#######################################################################################
void lib_3ds_draw_h_line(box_t *box, signed int X1, signed int X2, signed int Y, uint8_t width, uint8_t color) {
	register int X1_Tmp = X1, X2_Tmp = X1 + X2;
	if(box) {
		if(width == 1 && (Y < box->y_min || Y >= box->y_max))
			return;
		while(VIDEO.STATUS & STATUS_BUSY_BIT);
		VIDEO.SET_ACTION = ACTION_DRAW_PIXELS;
		if(X1_Tmp <= box->x_min)
			X1_Tmp = box->x_min;
		if(X1_Tmp >= box->x_max)
			X1_Tmp = box->x_max;
		if(X2_Tmp <= box->x_min)
			X2_Tmp = box->x_min;
		if(X2_Tmp >= box->x_max)
			X2_Tmp = box->x_max;
		if(Y < box->y_min)
			Y = box->y_min;
		if(Y >= box->y_max)
			Y = box->y_max;
	} else {
		while(VIDEO.STATUS & STATUS_BUSY_BIT);
		VIDEO.SET_ACTION = ACTION_DRAW_PIXELS;
	}
	//volatile int cnt = 1000;
	//while(cnt--);
	int Half_width1 = (width>>1);
	int Half_width2 = width-Half_width1;
	if(X2_Tmp - X1_Tmp > 0) {
		VIDEO.SET_WIN_START_X_H = X1_Tmp >> 8;
		VIDEO.SET_WIN_START_X_L = X1_Tmp;
		VIDEO.SET_WIN_END_X_H = X2_Tmp >> 8;
		VIDEO.SET_WIN_END_X_L = X2_Tmp;
		VIDEO.SET_WIN_START_Y_H = (Y - Half_width1) >> 8;
		VIDEO.SET_WIN_START_Y_L = Y - Half_width1;
		VIDEO.SET_WIN_END_Y_H = ((Y + Half_width2) - 1) >> 8;
		VIDEO.SET_WIN_END_Y_L = (Y + Half_width2) - 1;
		VIDEO.PIXEL = color;
	}
}
//#######################################################################################
void lib_3ds_draw_v_line(box_t *box, signed int Y1, signed int Y2, signed int X, uint8_t width, uint8_t color) {
	register int Y1_Tmp = Y1, Y2_Tmp = Y1 + Y2;
	if(box) {
		if(width == 1 && (X < box->x_min || X >= box->x_max))
			return;
		while(VIDEO.STATUS & STATUS_BUSY_BIT);
		VIDEO.SET_ACTION = ACTION_DRAW_PIXELS;
		if(X <= box->x_min)
			X = box->x_min;
		if(X >= box->x_max)
			X = box->x_max;
		if(Y1_Tmp <= box->y_min)
			Y1_Tmp = box->y_min;
		if(Y1_Tmp >= box->y_max)
			Y1_Tmp = box->y_max;
		if(Y2_Tmp <= box->y_min)
			Y2_Tmp = box->y_min;
		if(Y2_Tmp >= box->y_max)
			Y2_Tmp = box->y_max;
	} else {
		while(VIDEO.STATUS & STATUS_BUSY_BIT);
		VIDEO.SET_ACTION = ACTION_DRAW_PIXELS;
	}
	int Half_width1 = (width>>1);
	int Half_width2 = width - Half_width1;
	if(Y2_Tmp - Y1_Tmp > 0) {
		VIDEO.SET_WIN_START_X_H = (X - Half_width1) >> 8;
		VIDEO.SET_WIN_START_X_L = X - Half_width1;
		VIDEO.SET_WIN_END_X_H = ((X + Half_width2) - 1) >> 8;
		VIDEO.SET_WIN_END_X_L = (X + Half_width2) - 1;
		VIDEO.SET_WIN_START_Y_H = Y1_Tmp >> 8;
		VIDEO.SET_WIN_START_Y_L = Y1_Tmp;
		VIDEO.SET_WIN_END_Y_H = Y2_Tmp >> 8;
		VIDEO.SET_WIN_END_Y_L = Y2_Tmp;
		VIDEO.PIXEL = color;
	}
}
//#######################################################################################
void lib_3ds_draw_line(box_t *box, signed int X1, signed int Y1, signed int X2, signed int Y2, uint8_t width, uint8_t color) {
	if (width == 1) {
		signed int currentX, currentY, Xinc, Yinc, Dx, Dy, TwoDx, TwoDy,
				twoDxAccumulatedError, twoDyAccumulatedError;
		Dx = (X2 - X1);
		Dy = (Y2 - Y1);
		TwoDx = Dx + Dx;
		TwoDy = Dy + Dy;
		currentX = X1;
		currentY = Y1;
		Xinc = 1;
		Yinc = 1;
		if (Dx < 0) {
			Xinc = -1;
			Dx = -Dx;
			TwoDx = -TwoDx;
		}
		if (Dy < 0) {
			Yinc = -1;
			Dy = -Dy;
			TwoDy = -TwoDy;
		}
		lib_3ds_draw_pixel(box, X1, Y1, color);
		if ((Dx != 0) || (Dy != 0)) {
			if (Dy <= Dx) {
				twoDxAccumulatedError = 0;
				do {
					currentX += Xinc;
					twoDxAccumulatedError += TwoDy;
					if (twoDxAccumulatedError > Dx) {
						currentY += Yinc;
						twoDxAccumulatedError -= TwoDx;
					}
					lib_3ds_draw_pixel(box, currentX, currentY, color);
				} while (currentX != X2);
			}
			else {
				twoDyAccumulatedError = 0;
				do {
					currentY += Yinc;
					twoDyAccumulatedError += TwoDx;
					if (twoDyAccumulatedError > Dy) {
						currentX += Xinc;
						twoDyAccumulatedError -= TwoDy;
					}
					lib_3ds_draw_pixel(box, currentX, currentY, color);
				} while (currentY != Y2);
			}
		}
	} else {
		int32_t half_width;
		int32_t half_width_square;
		signed int dy, dx;
		signed int addx = 1, addy = 1, j;
		signed int P, diff;
		int32_t c1, c2;
		signed int i = 0;
		half_width = width >> 1;
		half_width_square = half_width;
		dx = X2 - X1;
		dy = Y2 - Y1;
		if(dx < 0) {
			dx = -dx;
			half_width_square = half_width * half_width;
		}
		if(dy < 0) {
			dy = -dy;
			half_width_square = half_width * half_width;
		}
		c1 = -(dx * X1 + dy * Y1);
		c2 = -(dx * X2 + dy * Y2);
		if (X1 > X2) {
			int32_t temp;
			temp = c1;
			c1 = c2;
			c2 = temp;
			addx = -1;
		}
		if (Y1 > Y2) {
			int32_t temp;
			temp = c1;
			c1 = c2;
			c2 = temp;
			addy = -1;
		}
		if (dx >= dy) {
			P = 2 * dy - dx;
			diff = P - dx;
			for (i = 0; i <= dx; ++i) {
#ifdef USE_FAST_DRAW_H_LINE
				lib_3ds_draw_v_line(box, Y1-(width - half_width), half_width, X1, 1, color);
#else
				for (j = -half_width; j < half_width + (width % 2); ++j) {
					int32_t temp;

					temp = dx * X1 + dy * (Y1 + j); // Use more RAM to increase speed
					if (temp + c1 >= -half_width_square && temp + c2 <= half_width_square)
						lib_3ds_draw_pixel(box, X1, Y1 + j, color);
				}
#endif
				if (P < 0) {
					P += 2 * dy;
					X1 += addx;
				}
				else {
					P += diff;
					X1 += addx;
					Y1 += addy;
				}
			}
		} else {
			P = 2 * dx - dy;
			diff = P - dy;
			for (i = 0; i <= dy; ++i) {
				if (P < 0) {
					P += 2 * dx;
					Y1 += addy;
				}
				else {
					P += diff;
					X1 += addx;
					Y1 += addy;
				}
#ifdef USE_FAST_DRAW_H_LINE
				lib_3ds_draw_h_line(box, X1-(width - half_width), half_width, Y1, 1, color);
#else
				for (j = -half_width; j < half_width + (width % 2); ++j) {
					signed int temp;

					temp = dx * X1 + dy * (Y1 + j); // Use more RAM to increase speed
					if (temp + c1 >= 0 && temp + c2 <= 0)
						lib_3ds_draw_pixel(box, X1 + j, Y1, color);
				}
#endif
			}
		}
	}
}

//#######################################################################################
void lib_3ds_draw_circle(box_t *box, signed int x, signed int y, uint16_t _radius, bool fill, uint8_t color){
	uint16_t radius = _radius;
	if (radius < 0)
		radius = ~radius;
	signed int a, b, P;
	a = 0;
	b = radius;
	P = 1 - radius;
	signed int Tmp1;
	signed int Tmp2;
	signed int Tmp3;
	signed int Tmp4;
	signed int Tmp5;
	signed int Tmp6;
	signed int Tmp7;
	signed int Tmp8;
	signed int _Tmp5 = 5, _Tmp7 = 0;
	do {
		Tmp1 = x + a;
		Tmp2 = x - a;
		Tmp3 = x + b;
		Tmp4 = x - b;
		Tmp5 = y + a;
		Tmp6 = y - a;
		Tmp7 = y + b;
		Tmp8 = y - b;
		if (fill) {
			if (_Tmp7 != Tmp7) {
				lib_3ds_draw_h_line(box, Tmp2, Tmp1 - Tmp2, Tmp7, 1, color);
				lib_3ds_draw_h_line(box, Tmp2, Tmp1 - Tmp2, Tmp8, 1, color);
			}
			if (_Tmp5 != Tmp5) {
				lib_3ds_draw_h_line(box, Tmp4, Tmp3 - Tmp4, Tmp5, 1, color);
				lib_3ds_draw_h_line(box, Tmp4, Tmp3 - Tmp4, Tmp6, 1, color);
			}
			_Tmp5 = Tmp5;
			_Tmp7 = Tmp7;
		} else {
			lib_3ds_draw_pixel(box, Tmp1, Tmp7, color);
			lib_3ds_draw_pixel(box, Tmp3, Tmp5, color);
			lib_3ds_draw_pixel(box, Tmp2, Tmp7, color);
			lib_3ds_draw_pixel(box, Tmp4, Tmp5, color);
			lib_3ds_draw_pixel(box, Tmp3, Tmp6, color);
			lib_3ds_draw_pixel(box, Tmp1, Tmp8, color);
			lib_3ds_draw_pixel(box, Tmp2, Tmp8, color);
			lib_3ds_draw_pixel(box, Tmp4, Tmp6, color);
		}
		if (P < 0)
			P += 3 + 2 * a++;
		else
			P += 5 + 2 * (a++ - b--);
	} while (a <= b);
}//#######################################################################################


#if __AVR_MEGA__
#include <avr/pgmspace.h>
static const int16_t lib_3ds_sin[] PROGMEM =  
#else
static const int16_t lib_3ds_sin[] =
#endif
	{
	    0,   571,  1143,  1714,  2285,  2855,  3425,  3993, 
	 4560,  5125,  5689,  6252,  6812,  7370,  7927,  8480, 
	 9031,  9580, 10125, 10667, 11206, 11742, 12274, 12803, 
	13327, 13847, 14364, 14875, 15383, 15885, 16383, 16876, 
	17363, 17846, 18323, 18794, 19259, 19719, 20173, 20620, 
	21062, 21497, 21925, 22347, 22761, 23169, 23570, 23964, 
	24350, 24729, 25100, 25464, 25820, 26168, 26509, 26841, 
	27165, 27480, 27787, 28086, 28377, 28658, 28931, 29195, 
	29450, 29696, 29934, 30162, 30381, 30590, 30790, 30981, 
	31163, 31335, 31497, 31650, 31793, 31927, 32050, 32164, 
	32269, 32363, 32448, 32522, 32587, 32642, 32687, 32722, 
	32747, 32762, 32767
	};
#if __AVR_MEGA__
#include <avr/pgmspace.h>
static const int16_t lib_3ds_cos[] PROGMEM = 
#else
static const int16_t lib_3ds_cos[] =
#endif
	{
	32767,32762,32747,32722,32687,32642,32587,32522,
	32448,32363,32269,32164,32050,31927,31793,31650,
	31497,31335,31163,30981,30790,30590,30381,30162,
	29934,29696,29450,29195,28931,28658,28377,28086,
	27787,27480,27165,26841,26509,26168,25820,25464,
	25100,24729,24350,23964,23570,23169,22761,22347,
	21925,21497,21062,20620,20173,19719,19259,18794,
	18323,17846,17363,16876,16383,15885,15383,14875,
	14364,13847,13327,12803,12274,11742,11206,10667,
	10125, 9580, 9031, 8480, 7927, 7370, 6812, 6252,
	 5689, 5125, 4560, 3993, 3425, 2855, 2285, 1714,
	 1143,  571,    0
	 };

int32_t get_sin(uint32_t deg) {
	uint32_t degIn = (uint32_t)(deg % 359);
#if __AVR_MEGA__
	if (degIn <= 90)
		return (int16_t)pgm_read_word(&lib_3ds_sin[degIn]);
	else if (degIn <= 180)
		return (int16_t)pgm_read_word(&lib_3ds_sin[180 - degIn]);
	else if (degIn <= 270)
		return -(int16_t)pgm_read_word(&lib_3ds_sin[degIn - 180]);
	else
		return -(int16_t)pgm_read_word(&lib_3ds_sin[360 - degIn]);
#else
	if (degIn <= 90)
		return lib_3ds_sin[degIn];
	else if (degIn <= 180)
		return lib_3ds_sin[180 - degIn];
	else if (degIn <= 270)
		return -lib_3ds_sin[degIn - 180];
	else
		return -lib_3ds_sin[360 - degIn];
#endif
}

int32_t get_cos(uint32_t deg) {
	uint32_t degIn = (uint32_t)(deg % 359);
#if __AVR_MEGA__
	if (degIn <= 90)
		return (int16_t)pgm_read_word(&lib_3ds_cos[degIn]);
	else if (degIn <= 180)
		return -(int16_t)pgm_read_word(&lib_3ds_cos[180 - degIn]);
	else if (degIn <= 270)
		return -(int16_t)pgm_read_word(&lib_3ds_cos[degIn - 180]);
	else
		return (int16_t)pgm_read_word(&lib_3ds_cos[360 - degIn]);
#else
	if (degIn <= 90)
		return lib_3ds_cos[degIn];
	else if (degIn <= 180)
		return -lib_3ds_cos[180 - degIn];
	else if (degIn <= 270)
		return -lib_3ds_cos[degIn - 180];
	else
		return lib_3ds_cos[360 - degIn];
#endif
}

//#######################################################################################
/*void make3DPoint(_3d_points *_point, uint32_t Cell, double x, double y, double z)
{
	_point->x[Cell] = x;
	_point->y[Cell] = y;
	_point->z[Cell] = z;
}*/
 //#######################################################################################
/*void make2DPoint(_3d_points *_point, uint32_t Cell, double x, double y, double depth, double scaleFactor)
{
	_point->x[Cell] = x;
	_point->y[Cell] = y;
	_point->depth[Cell] = depth;
	_point->scaleFactor[Cell] = scaleFactor;
}*/
//#######################################################################################
void lib_3ds_3DPointsTo2DPoints(_3d_points *screenPoints, _3d_points *Points, axisRotations AxisRotations) {
	double sx = sin(AxisRotations.x);
	double cx = cos(AxisRotations.x);
	double sy = sin(AxisRotations.y);
	double cy = cos(AxisRotations.y);
	double sz = sin(AxisRotations.z);
	double cz = cos(AxisRotations.z);
	double x,y,z, xy,xz, yx,yz, zx,zy, scaleFactor;
	uint32_t i = Points->pointsNr;
	while (i--){
		x = Points->x[i];
		y = Points->y[i];
		z = Points->z[i];
		// rotation around x
		xy = cx*y - sx*z;
		xz = sx*y + cx*z;
		// rotation around y
		yz = cy*xz - sy*x;
		yx = sy*xz + cy*x;
		// rotation around z
		zx = cz*yx - sz*xy;
		zy = sz*yx + cz*xy;
		scaleFactor = Points->focalLength /(Points->focalLength + yz);
		x = (zx*scaleFactor) / Points->depthScale;
		y = (zy*scaleFactor) / Points->depthScale;
		z = yz / Points->depthScale;
		//make2DPoint(screenPoints, i, x, y, -z, scaleFactor);
		screenPoints->x[i] = x;
		screenPoints->y[i] = y;
		screenPoints->depth[i] = -z;
		screenPoints->scaleFactor[i] = scaleFactor;
	}
	screenPoints->pointsNr = Points->pointsNr;
}
//#######################################################################################
void lib_3ds_3DPointsTo2DPoints_i(_2d_point_i *screenPoints, _3d_points_i *Points, axisRotations_i *AxisRotations) {
#if __AVR_MEGA__
	int16_t sx = get_sin(AxisRotations->x);
	int16_t cx = get_cos(AxisRotations->x);
	int16_t sy = get_sin(AxisRotations->y);
	int16_t cy = get_cos(AxisRotations->y);
	int16_t sz = get_sin(AxisRotations->z);
	int16_t cz = get_cos(AxisRotations->z);
#else
	int32_t sx = get_sin(AxisRotations->x);
	int32_t cx = get_cos(AxisRotations->x);
	int32_t sy = get_sin(AxisRotations->y);
	int32_t cy = get_cos(AxisRotations->y);
	int32_t sz = get_sin(AxisRotations->z);
	int32_t cz = get_cos(AxisRotations->z);
#endif
	int32_t x,y,z, xy,xz, yx,yz, zx,zy, scaleFactor;
	unsigned int i = Points->pointsNr;
	while (i--){
        x = Points->x[i];
        y = Points->y[i];
        z = Points->z[i];
        // rotation around x
        xz = (int32_t)((int32_t)(sx * y) + (int32_t)(cx * z)) / 65536;
        xy = (int32_t)((int32_t)(cx * y) - (int32_t)(sx * z)) / 65536;
        // rotation around y
        yx = (int32_t)((int32_t)(sy * xz) + (int32_t)(cy * x)) / 65536;
        yz = (int32_t)((int32_t)(cy * xz) - (int32_t)(sy * x)) / 65536;
        // rotation around z
        zy = (int32_t)((int32_t)(sz * yx) + (int32_t)(cz * xy)) / 65536;
        zx = (int32_t)((int32_t)(cz * yx) - (int32_t)(sz * xy)) / 65536;
        scaleFactor = (int32_t)((Points->focalLength * 65536) / ((Points->focalLength + yz)));
        x = (int32_t)((int32_t)(zx * scaleFactor) / (Points->depthScale * 65536));
        y = (int32_t)((int32_t)(zy * scaleFactor) / (Points->depthScale * 65536));
        z = (int32_t)((yz * 65536) / (Points->depthScale * 65536));
		//make2DPoint(screenPoints, i, x, y, -z, scaleFactor);
		screenPoints[i].x = (int32_t)x;
		screenPoints[i].y = (int32_t)y;
		screenPoints[i].depth = (int32_t)-z;
		screenPoints[i].scaleFactor = scaleFactor;
	}
}
//#######################################################################################
//#######################################################################################
//#######################################################################################
void lib_3ds_draw_triangle_i(box_t *box, _3d_points_i *Points, int16_t X_offset, int16_t Y_offset, uint16_t X_Angle, uint16_t Y_Angle, uint16_t Z_Angle, uint8_t color) {
	_2d_point_i screenPoints[3];
	axisRotations_i cubeAxisRotations;
	cubeAxisRotations.x = X_Angle;
	cubeAxisRotations.y = Y_Angle;
	cubeAxisRotations.z = Z_Angle;
	Points->pointsNr = 3;
	lib_3ds_3DPointsTo2DPoints_i(screenPoints, Points, &cubeAxisRotations);
	int16_t X_start = (int16_t)screenPoints[0].x;
	int16_t Y_start = (int16_t)screenPoints[0].y;
	int16_t X_end = (int16_t)screenPoints[1].x;
	int16_t Y_end = (int16_t)screenPoints[1].y;
	lib_3ds_draw_line(box, X_offset + X_start, Y_offset + Y_start, X_offset + X_end, Y_offset + Y_end, 1, color);
	X_start = (int16_t)screenPoints[1].x;
	Y_start = (int16_t)screenPoints[1].y;
	X_end = (int16_t)screenPoints[2].x;
	Y_end = (int16_t)screenPoints[2].y;
	lib_3ds_draw_line(box, X_offset + X_start, Y_offset + Y_start, X_offset + X_end, Y_offset + Y_end, 1, color);
	X_start = (int16_t)screenPoints[2].x;
	Y_start = (int16_t)screenPoints[2].y;
	X_end = (int16_t)screenPoints[0].x;
	Y_end = (int16_t)screenPoints[0].y;
	lib_3ds_draw_line(box, X_offset + X_start, Y_offset + Y_start, X_offset + X_end, Y_offset + Y_end, 1, color);
}
//#######################################################################################
void lib_3ds_draw_cube_i(box_t *box, _3d_points_i *Points, int32_t X_offset, int32_t Y_offset, double X_Angle, double Y_Angle, double Z_Angle, uint8_t Color) {
	_2d_point_i screenPoints[8];
	axisRotations_i cubeAxisRotations;
	cubeAxisRotations.y = Y_Angle;
	cubeAxisRotations.x = X_Angle;
	cubeAxisRotations.z = Z_Angle;
	Points->pointsNr = 8;
	lib_3ds_3DPointsTo2DPoints_i(screenPoints, Points, &cubeAxisRotations);
	lib_3ds_draw_line(box, (int16_t)screenPoints[0].x + X_offset, (int16_t)screenPoints[0].y + Y_offset, (int16_t)screenPoints[1].x + X_offset, (int16_t)screenPoints[1].y + Y_offset, 1, Color);
	lib_3ds_draw_line(box, (int16_t)screenPoints[0].x + X_offset, (int16_t)screenPoints[0].y + Y_offset, (int16_t)screenPoints[3].x + X_offset, (int16_t)screenPoints[3].y+ Y_offset, 1, Color);
	lib_3ds_draw_line(box, (int16_t)screenPoints[0].x + X_offset, (int16_t)screenPoints[0].y + Y_offset, (int16_t)screenPoints[4].x + X_offset, (int16_t)screenPoints[4].y + Y_offset, 1, Color);

	lib_3ds_draw_line(box, (int16_t)screenPoints[2].x + X_offset, (int16_t)screenPoints[2].y + Y_offset, (int16_t)screenPoints[1].x + X_offset, (int16_t)screenPoints[1].y + Y_offset, 1, Color);
	lib_3ds_draw_line(box, (int16_t)screenPoints[2].x + X_offset, (int16_t)screenPoints[2].y + Y_offset, (int16_t)screenPoints[3].x + X_offset, (int16_t)screenPoints[3].y + Y_offset, 1, Color);
	lib_3ds_draw_line(box, (int16_t)screenPoints[2].x + X_offset, (int16_t)screenPoints[2].y + Y_offset, (int16_t)screenPoints[6].x + X_offset, (int16_t)screenPoints[6].y + Y_offset, 1, Color);

	lib_3ds_draw_line(box, (int16_t)screenPoints[5].x + X_offset, (int16_t)screenPoints[5].y + Y_offset, (int16_t)screenPoints[1].x + X_offset, (int16_t)screenPoints[1].y + Y_offset, 1, Color);
	lib_3ds_draw_line(box, (int16_t)screenPoints[5].x + X_offset, (int16_t)screenPoints[5].y + Y_offset, (int16_t)screenPoints[4].x + X_offset, (int16_t)screenPoints[4].y + Y_offset, 1, Color);
	lib_3ds_draw_line(box, (int16_t)screenPoints[5].x + X_offset, (int16_t)screenPoints[5].y + Y_offset, (int16_t)screenPoints[6].x + X_offset, (int16_t)screenPoints[6].y + Y_offset, 1, Color);

	lib_3ds_draw_line(box, (int16_t)screenPoints[7].x + X_offset, (int16_t)screenPoints[7].y + Y_offset, (int16_t)screenPoints[3].x + X_offset, (int16_t)screenPoints[3].y + Y_offset, 1, Color);
	lib_3ds_draw_line(box, (int16_t)screenPoints[7].x + X_offset, (int16_t)screenPoints[7].y + Y_offset, (int16_t)screenPoints[4].x + X_offset, (int16_t)screenPoints[4].y + Y_offset, 1, Color);
	lib_3ds_draw_line(box, (int16_t)screenPoints[7].x + X_offset, (int16_t)screenPoints[7].y + Y_offset, (int16_t)screenPoints[6].x + X_offset, (int16_t)screenPoints[6].y + Y_offset, 1, Color);
}
//#######################################################################################
//#######################################################################################
//#######################################################################################
void lib_3ds_draw_triangle(box_t *box, _3d_points *Points, int32_t X_offset, int32_t Y_offset, double X_Angle, double Y_Angle, double Z_Angle, uint8_t color) {
	_3d_points screenPoints;
	axisRotations cubeAxisRotations;
	cubeAxisRotations.y = Y_Angle;
	cubeAxisRotations.x = X_Angle;
	cubeAxisRotations.z = Z_Angle;
	Points->pointsNr = 4;
	lib_3ds_3DPointsTo2DPoints(&screenPoints, Points, cubeAxisRotations);
	int32_t X_start = (int32_t)screenPoints.x[1];
	int32_t Y_start = (int32_t)screenPoints.y[1];
	int32_t X_end = (int32_t)screenPoints.x[2];
	int32_t Y_end = (int32_t)screenPoints.y[2];
	lib_3ds_draw_line(box, X_offset + X_start, Y_offset + Y_start, X_offset + X_end, Y_offset + Y_end, 1, color);
	X_start = (int32_t)screenPoints.x[2];
	Y_start = (int32_t)screenPoints.y[2];
	X_end = (int32_t)screenPoints.x[3];
	Y_end = (int32_t)screenPoints.y[3];
	lib_3ds_draw_line(box, X_offset + X_start, Y_offset + Y_start, X_offset + X_end, Y_offset + Y_end, 1, color);
	X_start = (int32_t)screenPoints.x[3];
	Y_start = (int32_t)screenPoints.y[3];
	X_end = (int32_t)screenPoints.x[1];
	Y_end = (int32_t)screenPoints.y[1];
	lib_3ds_draw_line(box, X_offset + X_start, Y_offset + Y_start, X_offset + X_end, Y_offset + Y_end, 1, color);
}
//#######################################################################################
void lib_3ds_draw_rectangle(box_t *box, _3d_points *Points, int32_t X_offset, int32_t Y_offset, double X_Angle, double Y_Angle, double Z_Angle, uint8_t Color) {
	_3d_points screenPoints;
	axisRotations cubeAxisRotations;
	cubeAxisRotations.y = Y_Angle;
	cubeAxisRotations.x = X_Angle;
	cubeAxisRotations.z = Z_Angle;
	Points->pointsNr = 8;
	lib_3ds_3DPointsTo2DPoints(&screenPoints, Points, cubeAxisRotations);
	lib_3ds_draw_line(box, (int32_t)screenPoints.x[0] + X_offset, (int32_t)screenPoints.y[0] + Y_offset, (int32_t)screenPoints.x[1] + X_offset, (int32_t)screenPoints.y[1] + Y_offset, 1, Color);
	lib_3ds_draw_line(box, (int32_t)screenPoints.x[0] + X_offset, (int32_t)screenPoints.y[0] + Y_offset, (int32_t)screenPoints.x[3] + X_offset, (int32_t)screenPoints.y[3] + Y_offset, 1, Color);
	lib_3ds_draw_line(box, (int32_t)screenPoints.x[0] + X_offset, (int32_t)screenPoints.y[0] + Y_offset, (int32_t)screenPoints.x[4] + X_offset, (int32_t)screenPoints.y[4] + Y_offset, 1, Color);

	lib_3ds_draw_line(box, (int32_t)screenPoints.x[2] + X_offset, (int32_t)screenPoints.y[2] + Y_offset, (int32_t)screenPoints.x[1] + X_offset, (int32_t)screenPoints.y[1] + Y_offset, 1, Color);
	lib_3ds_draw_line(box, (int32_t)screenPoints.x[2] + X_offset, (int32_t)screenPoints.y[2] + Y_offset, (int32_t)screenPoints.x[3] + X_offset, (int32_t)screenPoints.y[3] + Y_offset, 1, Color);
	lib_3ds_draw_line(box, (int32_t)screenPoints.x[2] + X_offset, (int32_t)screenPoints.y[2] + Y_offset, (int32_t)screenPoints.x[6] + X_offset, (int32_t)screenPoints.y[6] + Y_offset, 1, Color);

	lib_3ds_draw_line(box, (int32_t)screenPoints.x[5] + X_offset, (int32_t)screenPoints.y[5] + Y_offset, (int32_t)screenPoints.x[1] + X_offset, (int32_t)screenPoints.y[1] + Y_offset, 1, Color);
	lib_3ds_draw_line(box, (int32_t)screenPoints.x[5] + X_offset, (int32_t)screenPoints.y[5] + Y_offset, (int32_t)screenPoints.x[4] + X_offset, (int32_t)screenPoints.y[4] + Y_offset, 1, Color);
	lib_3ds_draw_line(box, (int32_t)screenPoints.x[5] + X_offset, (int32_t)screenPoints.y[5] + Y_offset, (int32_t)screenPoints.x[6] + X_offset, (int32_t)screenPoints.y[6] + Y_offset, 1, Color);

	lib_3ds_draw_line(box, (int32_t)screenPoints.x[7] + X_offset, (int32_t)screenPoints.y[7] + Y_offset, (int32_t)screenPoints.x[3] + X_offset, (int32_t)screenPoints.y[3] + Y_offset, 1, Color);
	lib_3ds_draw_line(box, (int32_t)screenPoints.x[7] + X_offset, (int32_t)screenPoints.y[7] + Y_offset, (int32_t)screenPoints.x[4] + X_offset, (int32_t)screenPoints.y[4] + Y_offset, 1, Color);
	lib_3ds_draw_line(box, (int32_t)screenPoints.x[7] + X_offset, (int32_t)screenPoints.y[7] + Y_offset, (int32_t)screenPoints.x[6] + X_offset, (int32_t)screenPoints.y[6] + Y_offset, 1, Color);
}
//#######################################################################################



