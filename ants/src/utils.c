#include <SDL2/SDL.h>

#include <stdlib.h>
#include <math.h>

#include "utils.h"

extern SDL_Renderer* renderer;

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

void draw_vector(v2* a, v2* b) {  
    v2 n = *b;
    v2_nrm(&n);
    v2_mul(&n, 6.0);
    v2 t = {b->y, -b->x};
    v2_nrm(&t);
    v2_mul(&t, 3.0);
    v2 p0 = *a;
    v2 p1 = {a->x + b->x, a->y + b->y};
    /*
    SDL_Point points[] = {
        {p0.x, p0.y},
        {p1.x, p1.y},
        {p1.x - n.x + t.x, p1.y - n.y + t.y},
        {p1.x - n.x - t.x, p1.y - n.y - t.y}, 
        {p1.x, p1.y},
    };
    SDL_SetRenderDrawColor(renderer, 128, 128, 128, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawLines(renderer, points, 5);
    */
}

double rnd(double r) {
    return rand()%1000/1000.0*r;
}
