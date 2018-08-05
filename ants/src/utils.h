#pragma once

#define WIDTH  800
#define HEIGHT 800

#define PI  3.1415926
#define PI2 PI*2.0

typedef struct v2 {
    float x, y;
} v2;

void clamp_angle(float* a);
void v2_mul(v2* a, float k);
float v2_len(v2* a);
float rnd(float r);

void draw_vector(v2* a, v2* b);
