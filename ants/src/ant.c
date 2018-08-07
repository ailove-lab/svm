#include <SDL2/SDL.h>

#include "utils.h"
#include "world.h"
#include "brain.h"
#include "ant.h"
#include "food.h"

#define a_id (ant->id  )
#define a_d  (ant->data)

// Y
#define a_fx (a_d[0])
#define a_fy (a_d[1])
#define a_fa (a_d[2])

// X Vision 
#define a_vs (&a_d[3])

// X Introspection
#define a_vx (ant->v.y)
#define a_vy (ant->v.x)
#define a_va (ant->va )
#define a_sc (ant->sc )

#define a_p  (ant->p  )
#define a_px (ant->p.x)
#define a_py (ant->p.y)
#define a_a  (ant->a  )
#define a_n  (ant->n  )
#define a_nx (ant->n.x)
#define a_ny (ant->n.y)
#define a_m  (ant->m  )

#define a_mem (ant->memory)
#define a_mid (ant->mid)
#define a_max_dsc (ant->max_dsc)

#define a_b (ant->brain)

extern SDL_Renderer* renderer;

static double friction = 0.95;

static void print_data(ant_t* ant) {
   printf("["); 
   for(int i=0; i<Y_COUNT; i++) printf("% 4.2f ", a_d[i]); 
   printf("] < [");
   for(int i=Y_COUNT; i< Y_COUNT + X_COUNT; i++) printf("% 4.2f ", a_d[i]);
   printf("]\n"); 
}


static void think(ant_t* ant) {
    // simple attractor
    // if (true) {
    if(!a_b->trained) {
        v2 f = {0.0, 0.0};
        for(int i=0; i<VISION_RESOLUTION; i++) {
            double a = (double)i/VISION_RESOLUTION*PI2-PI;
            f.x+= cos(a)*a_vs[i];
            f.y+= sin(a)*a_vs[i];
        }
        v2_nrm(&f);
        a_fx = f.x/20.0+rnd(0.02)-0.01;
        a_fy = f.y/20.0+rnd(0.02)-0.01;
        a_fa = -atan2(f.y, f.x)/1000.0;
    } else {
        // print_data(ant);
        brain_predict(a_b, a_d);
        if(isnan(a_fx)) a_fx=0.0;
        if(isnan(a_fy)) a_fy=0.0;
        if(isnan(a_fa)) a_fa=0.0;
    };
}

static void update(ant_t* ant) { 

    // linear motion
    a_vx+= a_fx/ant->m;
    a_vy+= a_fy/ant->m;
    a_vx*= friction; 
    a_vy*= friction;

    // project from local to world
    v2 v = {a_vx, a_vy};
    v2_rot(&v, ant->a);
    
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
    a_nx = cos(a_a);
    a_ny = sin(a_a);
}

static void render(ant_t* ant) {
    if (a_b->trained) {
        SDL_SetRenderDrawColor(renderer, 128, 255, 128, SDL_ALPHA_OPAQUE);
    } else {
        SDL_SetRenderDrawColor(renderer, 255, 128, 128, SDL_ALPHA_OPAQUE);
    };
    double s = 10.0;
    SDL_Point points[] = {
       {a_px + a_nx*s             , a_py + a_ny*s             },
       {a_px - a_nx*s - a_ny*s/2.0, a_py - a_ny*s + a_nx*s/2.0},
       {a_px - a_nx*s + a_ny*s/2.0, a_py - a_ny*s - a_nx*s/2.0}, 
       {a_px + a_nx*s             , a_py + a_ny*s             },
    };
    SDL_RenderDrawLines(renderer, points, 4);
}


// map some vector+value to perception array
static void perception_map(ant_t* ant, v2 t, double v) {

    t = (v2){t.x - a_px, t.y - a_py};
    double l = v2_len(&t)/VISION_RANGE;
    v = v/(1.0+l*l);
    
    // angle of target vector
    double a1 = atan2(a_ny, a_nx);
    double a2 = atan2(t.y, t.x);
    double a3 = a1-a2;
    clamp_angle(&a3);
    a3/=PI2/VISION_RESOLUTION;
    a3+=VISION_RESOLUTION/2;
    // antialias angle between near cells
    int i0 = floor(a3);
    int i1 = ceil (a3);
    double k0 = i1-a3;
    double k1 = a3-i0;
    i1%=VISION_RESOLUTION;
    // printf("% 4.2f [% 02d % 4.2f] [% 02d % 4.2f]", a3, i0, k0, i1, k1);       
    // angle between head vector and target vector
    a_vs[i0]+=v*k0;
    a_vs[i1]+=v*k1;
}

static void vision_update(ant_t* ant, world_t* world) {

    for(int i=0; i<VISION_RESOLUTION; i++) a_vs[i] = 0.0;

    ant_t** ants = world->ants; 
	for(int i=0; i<world->ants_count; i++) {
        ant_t* aa = ants[i];
        // the same ant
        if(a_px == aa->p.x && a_py == aa->p.y) continue;
        perception_map(ant, aa->p,-1.0);
        // toroidal symmetry 
        perception_map(ant, (v2){aa->p.x-WIDTH, aa->p.y }, -0.5);
        perception_map(ant, (v2){aa->p.x+WIDTH, aa->p.y }, -0.5);
        perception_map(ant, (v2){aa->p.x, aa->p.y-HEIGHT}, -0.5);
        perception_map(ant, (v2){aa->p.x, aa->p.y+HEIGHT}, -0.5);
	}
    
	food_t* food = world->food;
	for(int i=0; i<world->food_count; i++) {
        perception_map(ant, food[i].p, 1.0);
        // toroidal symmetry
        perception_map(ant, (v2){food[i].p.x-WIDTH, food[i].p.y }, 1.0);
        perception_map(ant, (v2){food[i].p.x+WIDTH, food[i].p.y }, 1.0);
        perception_map(ant, (v2){food[i].p.x, food[i].p.y-HEIGHT}, 1.0);
        perception_map(ant, (v2){food[i].p.x, food[i].p.y+HEIGHT}, 1.0);
	}

	double old_sc = a_sc;
	a_sc = 0.0;
	for(int i=0; i<VISION_RESOLUTION; i++) {
        // sigmoid
    	a_vs[i] = tanh(a_vs[i]);
        a_sc += a_vs[i];//*cos(PI+(double)i/VISION_RESOLUTION*PI2); 
	}
	
	// delta score
    double dsc = a_sc - old_sc;
	if((!a_b->trained && dsc>0.1)|| (a_b->trained && dsc>a_max_dsc)){
    	printf("%d (%03d: % 3.2f + % 3.2f)\n", a_id, a_mid, old_sc, dsc);
    	a_max_dsc = dsc;
    	memcpy(&a_mem[a_mid*MEMORY_STEP], a_d, sizeof(double)*MEMORY_STEP);
    	a_mid = (a_mid+1)%MEMORY_SIZE;
    	if(a_mid == 0) {
        	printf("train\n");
        	brain_train(a_b, a_mem, MEMORY_SIZE);
        	char buf[256];
        	sprintf(buf, "ant.%d", a_id);
        	brain_save(a_b, buf);
    	}
	}
}

static void ant_vision_render(ant_t* ant) {
    Uint8 r,g,b;
    b = 48;
    v2 f = {a_fx, a_fy};
    v2_rot(&f, a_a); v2_mul(&f, 1000.0);
    // draw_vector(&a_p, &f);
    int d = 8;
	for(int i=0; i<VISION_RESOLUTION; i++) {
        if      (a_vs[i] > 0.0) { r=0            ; g = a_vs[i]*255;}
        else if (a_vs[i] < 0.0) { r =-a_vs[i]*255; g = 0         ;}
        else                    { r = 0          ; g = 0         ;}
        SDL_Rect rect = {d+i*(d+1), d+a_id*(d+1), d, d};
        SDL_SetRenderDrawColor(renderer, r, g, b, SDL_ALPHA_OPAQUE);
        SDL_RenderFillRect(renderer, &rect);
	}
	d = 4;
	for(int i=0; i<VISION_RESOLUTION; i++) {
    	// a_vs[i] = cos(PI+(double)i/VISION_RESOLUTION*PI2);
        if      (a_vs[i] > 0.0) { r=0            ; g = a_vs[i]*255;}
        else if (a_vs[i] < 0.0) { r =-a_vs[i]*255; g = 0         ;}
        else                    { r = 0          ; g = 0         ;}
        double a = (0.5-(double)i/VISION_RESOLUTION)*PI*2.0;
        double dx = 30.0*cos(a+a_a);
        double dy = 30.0*sin(a+a_a);
        SDL_Rect rect = {a_px+dx-d/2, a_py+dy-d/2, d, d};
        SDL_SetRenderDrawColor(renderer, r, g, b, SDL_ALPHA_OPAQUE);
        SDL_RenderFillRect(renderer, &rect);
	}
}

/// INTERFACE ///

static int id;
ant_t* ant_create() {
    ant_t* ant = calloc(1, sizeof(ant_t));
    a_id = id++;
    a_b = brain_create(Y_COUNT, X_COUNT);
    brain_load(a_b, "basic");
    a_px = rand()%WIDTH;
    a_py = rand()%HEIGHT;
    a_m  = 10.0;
    return ant;
}

void ant_delete(ant_t* ant) {
    free(a_b);
    free(ant);
}

void ants_update(world_t* world) { 
	for(int i=0; i<world->ants_count; i++) {
        vision_update(world->ants[i], world);
        think(world->ants[i]);
        update(world->ants[i]);
	}
}

void ants_render(ant_t** ants, int n) {
    for(int i=0; i<n; i++){
        render(ants[i]);
    }
}

void ants_vision_render(ant_t** ants, int n) {
	for(int i=0; i<n; i++) {
    	ant_vision_render(ants[i]);
	}
}
