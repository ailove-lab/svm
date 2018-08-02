#include "utils.h"

void v2_mul(v2* a, float k) {
    a->x*=k;
    a->y*=k;
}

float rnd(float r) {return rand()%1000/1000.0*r;}
