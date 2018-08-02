#include <SDL2/SDL.h>

#include "world.h"
#include "ant.h"
#include "food.h"

static float friction = 0.95;

static void ant_update(ant_t* a) {    
    // linear motion
    a->v.x+= a->f.x/a->m;
    a->v.y+= a->f.y/a->m;
    a->v.x*= friction; 
    a->v.y*= friction;
    a->p.x+= a->v.x;
    a->p.y+= a->v.y;
    if(a->p.x > WIDTH ) a->p.x-=WIDTH;
    if(a->p.y > HEIGHT) a->p.y-=HEIGHT;
    if(a->p.x < 0) a->p.x+=WIDTH;
    if(a->p.y < 0) a->p.y+=HEIGHT;

    // rotation
    a->av+= a->af/a->m;
    a->av*= friction;
    a->a += a->av;
    if(a->a > PI2) a->a-=PI2;
    if(a->a < 0  ) a->a+=PI2;

    // normal
    a->n.x = sin(a->a);
    a->n.y = cos(a->a);
}

static void ant_render(ant_t* a) {
    float s = 10.0;
    v2 p = a->p;
    v2 n = a->n;
    v2_mul(&n, s);
    v2 t = {n.y, -n.x}; // perpendiuclar to normal
    SDL_Point points[] = {
       {p.x + n.x, p.y + n.y},
       {p.x - n.x - t.x/2.0, p.y - n.y-t.y/2.0},
       {p.x - n.x + t.x/2.0, p.y - n.y+t.y/2.0}, 
       {p.x + n.x, p.y + n.y},
    };
    SDL_RenderDrawLines(renderer, points, 4);
}

static void ant_vision_update(ant_t* a, world_t* w) {

    for(int i=0; i<100; i++) a->vision[i] = 0.0;
    
    v2 p0 = a->p;
    ants_t* ants = world->ants; 
	for(int i=0; i<world->ans_count; i++) {
       v2 p1 = ants[i].p;
       // the same ant
       if(p0.x == p1.x && p0.y == p1.y) continue;
       // vector from ant to another ant
       v2 t = {p1.x-p0.x, p1.y-p0.y};
       // angle of target vector
       float a2 = atan2(t.y, t.x);
       // angle between head vector and target vector
       int angle = (t2 - a->a)/PI*100;
       a->vision[angle]-=0.2;
	}

	food_t* food = world->food;
	for(int i=0; i<world->food_count; i++) {
       v2 p1 = food[i].p;
       // vector from ant to food
       v2 t = {p1.x-p0.x, p1.y-p0.y};
       // angle of target vector
       float a2 = atan2(t.y, t.x);
       // angle between head vector and target vector
       int angle = (t2 - a->a)/PI*100;
       a->vision[angle]+=0.2;

	}
}

static void ants_vision_update(world_t* world) {
    for(int i=0; i<n; i++){
        ant_vision_update(&ants[i], world);
    }
}

static void ant_vision_render(ant_t& a, int y) {
    float* v = a->vision;
    int r,g,b,a;
    b = 128;
    a = 255;
	for(int i=0; i<100; i++) {
        if(v[i] > 0.0) g = v[i]*255;
        if(v[i] < 0.0) r =-v[i]*255;
        SDL_Rect r = {i*5, y, 4, 4};
        SDL_SetRenderDrawColor(renderer, r, g, b, a);
        SDL_RenderFillRect(renderer, r);
	}
}

static void ant_kick(ant_t* a) {
    if (rand()%100>95) a->f = a->n;
    else a->f = (v2){0.0, 0.0};  
    if (rand()%100>95) a->af = (50-rand()%100)/500.0;
    else a->af = 0.0; 
}

/// INTERFACE ///

void ants_update(world_t* world) { 
	for(int i=0; i<world->ants_count; i++) {
        ant_kick(&world->ants[i]);
        ant_vision_update(&world->ants[i], world)
        ant_update(&world->ants[i]);
	}
}

void ants_render(ant_t* ants, int n) {
    SDL_SetRenderDrawColor(renderer, 255, 128, 128, SDL_ALPHA_OPAQUE);
    for(int i=0; i<n; i++){
        ant_render  (&ants[i]);
    }
}

void ants_vision_render(ant_t* ants, int n) {
	for(int i=0; i<10; i++) {
    	ant_vision_render(&ants[i], i*5);
	}
}
