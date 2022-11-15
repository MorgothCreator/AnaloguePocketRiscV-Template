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


#ifndef _3D_H_
#define _3D_H_

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#define _depth 1

#define PI (3.14159265358979323846264338327950288)
#define PI2 (3.14159265358979323846264338327950288/2.0)


typedef struct {
	int16_t x_min;
	int16_t x_max;
	int16_t y_min;
	int16_t y_max;
}box_t;


typedef struct {
	int32_t x;
	int32_t y;
	int32_t depth;
	int32_t scaleFactor;
}_2d_point_i;

typedef struct {
	int32_t *x;
	int32_t *y;
	int32_t *z;
	int32_t *depth;
	int32_t *scaleFactor;
	int32_t focalLength;
	int32_t depthScale;
	uint16_t pointsNr;
}_3d_points_i;

typedef struct 
{
	uint32_t x;
	uint32_t y;
	uint32_t z;
}axisRotations_i;

typedef struct
{
	double *x;
	double *y;
	double *z;
	double *depth;
	double *scaleFactor;
	double focalLength;
	double depthScale;
	uint32_t pointsNr;
}point;

typedef struct
{
	double *x;
	double *y;
	double *z;
	double *depth;
	double *scaleFactor;
	double focalLength;
	double depthScale;
	uint32_t pointsNr;
}_3d_points;

typedef struct
{
	double x;
	double y;
	double z;
}axisRotations;

typedef struct
{
	point points[8];
}Transformed_Points_Array;

void lib_3ds_draw_pixel(box_t *box, int16_t x, int16_t y, uint8_t color);
void lib_3ds_clear(uint8_t color);
void lib_3ds_draw_line(box_t *box, signed int X1, signed int Y1, signed int X2, signed int Y2, uint8_t width, uint8_t color);
void lib_3ds_draw_circle(box_t *box, signed int x, signed int y, uint16_t _radius, bool fill, uint8_t color);
void lib_3ds_3DPointsTo2DPoints(_3d_points *screenPoints, _3d_points *Points, axisRotations AxisRotations);
void lib_3ds_draw_triangle(box_t *box, _3d_points *Points, int32_t X_offset, int32_t Y_offset, double X_Angle, double Y_Angle, double Z_Angle, uint8_t Color);
void lib_3ds_draw_triangle_i(box_t *box, _3d_points_i *Points, int16_t X_offset, int16_t Y_offset, uint16_t X_Angle, uint16_t Y_Angle, uint16_t Z_Angle, uint8_t color);
void lib_3ds_draw_rectangle(box_t *box, _3d_points *Points, int32_t X_offset, int32_t Y_offset, double X_Angle, double Y_Angle, double Z_Angle, uint8_t Color);

//#######################################################################################
#ifdef USE_VISUAL_STUDIO
#include "3d.cpp"
#endif
#endif /* 3D_H_ */
