#pragma once

#define PI  3.1415926
#define PI2 PI*2.0

typedef struct {
    float x, y;
} v2;

void v2_mul(v2* a, float k);
float rnd(float r);
