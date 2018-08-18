#include <stdlib.h>
#include <math.h>

#include "utils.h"

void v2_mul(v2* a, double k) {
    a->x*=k;
    a->y*=k;
}

double v2_len(v2* a) {
    return sqrt(a->x*a->x + a->y*a->y);
}

void v2_nrm(v2* a) {
    double l = v2_len(a);
    a->x/=l;
    a->y/=l;
}

void clamp_angle(double* a) {
    if(*a > PI) *a-=PI2;
    if(*a <-PI) *a+=PI2;
}

void v2_rot(v2* a, double b) {
    *a = (v2){
        a->x*cos(b) - a->y*sin(b),
        a->x*sin(b) - a->y*cos(b),
    };
}

double rnd(double r) {
    return (double)rand()/(double)RAND_MAX*r;
}

