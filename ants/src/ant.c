#include <SDL2/SDL.h>

#include "utils.h"
#include "world.h"
#include "ant.h"
#include "food.h"

#define a_f  (ant->features)
#define a_vx (a_f[VISION_RESOLUTION+0])
#define a_vy (a_f[VISION_RESOLUTION+1])
#define a_va (a_f[VISION_RESOLUTION+2])
#define a_fx (a_f[VISION_RESOLUTION+3])
#define a_fy (a_f[VISION_RESOLUTION+4])
#define a_fa (a_f[VISION_RESOLUTION+5])
#define a_sc (a_f[VISION_RESOLUTION+6])
#define a_px (ant->p.x)
#define a_py (ant->p.y)
#define a_a  (ant->a  )
#define a_n  (ant->n  )
#define a_nx (ant->n.x)
#define a_ny (ant->n.y)

extern SDL_Renderer* renderer;

static float friction = 0.95;

static void update(ant_t* ant) { 

    // linear motion
    a_vx+= a_fx/ant->m;
    a_vy+= a_fy/ant->m;
    a_vx*= friction; 
    a_vy*= friction;

    // project from local to world
    v2 v = {
        a_vx*cos(ant->a) - a_vy*sin(ant->a),
        a_vx*sin(ant->a) - a_vy*cos(ant->a),
    };
    
    a_px += v.x;
    a_py += v.y;
    if(a_px > WIDTH ) a_px-=WIDTH;
    if(a_py > HEIGHT) a_py-=HEIGHT;
    if(a_px <      0) a_px+=WIDTH;
    if(a_py <      0) a_py+=HEIGHT;

    // rotation
    a_va+= a_fa/ant->m;
    a_va*= friction;
    ant->a+= a_va;
    clamp_angle(&a_a);
    // normal
    a_nx = sin(a_a);
    a_ny = cos(a_a);
}

static void render(ant_t* ant) {
    float s = 10.0;
    SDL_Point points[] = {
       {a_px + a_nx*s             , a_py + a_ny*s             },
       {a_px - a_nx*s - a_ny*s/2.0, a_py - a_ny*s + a_nx*s/2.0},
       {a_px - a_nx*s + a_ny*s/2.0, a_py - a_ny*s - a_nx*s/2.0}, 
       {a_px + a_nx*s             , a_py + a_ny*s             },
    };
    SDL_RenderDrawLines(renderer, points, 4);
}


// map some vector+value to perception array
static void perception_map(ant_t* ant, v2 t, float v) {
   // angle of target vector
   float a1 = atan2(a_ny, a_nx);
   float a2 = atan2(t.y, t.x);
   float a3 = a2-a1;
   clamp_angle(&a3);
   a3/=PI2/VISION_RESOLUTION;
   a3+=VISION_RESOLUTION/2;
   // antialias angle between near cells
   int i0 = floor(a3);
   int i1 = ceil (a3);
   float k0 = i1-a3;
   float k1 = a3-i0;
   i1%=VISION_RESOLUTION;
   // printf("% 4.2f [% 02d % 4.2f] [% 02d % 4.2f]", a3, i0, k0, i1, k1);       
   // angle between head vector and target vector
   a_f[i0]+=v*k0;
   a_f[i1]+=v*k1;
}

static void vision_update(ant_t* ant, world_t* world) {

    for(int i=0; i<VISION_RESOLUTION; i++) a_f[i] = 0.0;

    ant_t* ants = world->ants; 
	for(int i=0; i<world->ants_count; i++) {
        ant_t* aa = &ants[i];
        // the same ant
        if(a_px == aa->p.x && a_py == aa->p.y) continue;
        v2 t = {aa->p.x - a_px, aa->p.y - a_py};
        float l = v2_len(&t)/VISION_RANGE;
        float v = -1.0/(1.0+l*l);
        perception_map(ant, t, v);
	}
    
	food_t* food = world->food;
	for(int i=0; i<world->food_count; i++) {
        // vector from ant to food
        v2 t = {food[i].p.x - a_px, food[i].p.y - a_py};
        float l = v2_len(&t)/VISION_RANGE;
        float v = 1.0/(1.0+l*l);
        perception_map(ant, t, v);
	}

	a_sc = 0.0;
	for(int i=0; i<VISION_RESOLUTION; i++) {
        a_sc += a_f[i]; 
	}
	
}

static void ant_vision_render(ant_t* ant) {
    Uint8 r,g,b;
    b = 48;
	for(int i=0; i<VISION_RESOLUTION; i++) {
        if      (a_f[i] > 0.0) { r=0           ; g = a_f[i]*255;}
        else if (a_f[i] < 0.0) { r =-a_f[i]*255; g = 0         ;}
        else                   { r = 0         ; g = 0         ;}
        float a = (0.5-(float)i/VISION_RESOLUTION)*PI*2.0;
        float dx = 30.0*sin(a+a_a);
        float dy = 30.0*cos(a+a_a);
        int d = 4;
        SDL_Rect rect = {a_px+dx-d/2, a_py+dy-d/2, d, d};
        SDL_SetRenderDrawColor(renderer, r, g, b, SDL_ALPHA_OPAQUE);
        SDL_RenderFillRect(renderer, &rect);
	}
}

static void ant_kick(ant_t* ant) {
    if (rand()%100>95) a_fy = -1.0;
    else a_fy = 0.0;
    
    if (rand()%100>95) a_fa = (50-rand()%100)/500.0;
    else a_fa = 0.0;
}

/// INTERFACE ///

void ants_update(world_t* world) { 
	for(int i=0; i<world->ants_count; i++) {
        ant_kick(&world->ants[i]);
        update(&world->ants[i]);
        vision_update(&world->ants[i], world);
	}
}

void ants_render(ant_t* ants, int n) {
    SDL_SetRenderDrawColor(renderer, 255, 128, 128, SDL_ALPHA_OPAQUE);
    for(int i=0; i<n; i++){
        render  (&ants[i]);
    }
}

void ants_vision_render(ant_t* ants, int n) {
	for(int i=0; i<n; i++) {
    	ant_vision_render(&ants[i]);
	}
}
