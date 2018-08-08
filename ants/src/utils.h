#pragma once

#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#define WIDTH  800
#define HEIGHT 800

#define PI  3.1415926
#define PI2 PI*2.0

typedef struct v2 {
    double x, y;
} v2;

void clamp_angle(double* a);
void   v2_mul(v2* a, double k);
void   v2_nrm(v2* a);
void   v2_rot(v2* a, double b);
double v2_len(v2* a);
double rnd(double r);

void draw_vector(v2* a, v2* b);
