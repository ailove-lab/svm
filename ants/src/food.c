#include <math.h>
#include <stdlib.h>
#include <nanovg.h>


#include "globals.h"
#include "food.h"

#define f_size  (food->size)
#define f_mass  (food->mass)
#define f_body  (food->body)
#define f_shape (food->shape)

#define NUM_VERTS 3

food_t* foodNew(cpSpace* space) {
    food_t* food = calloc(1, sizeof(food_t));

    f_size = 5.0;
    f_mass = 1.0;

	cpVect verts[NUM_VERTS];
     
	for(int i=0; i<NUM_VERTS; i++){
		cpFloat angle = -2.0f*CP_PI*i/((cpFloat) NUM_VERTS);
		verts[i] = cpv(f_size*cos(angle), f_size*sin(angle));
	}
		
    f_body = cpSpaceAddBody(space, cpBodyNew(f_mass, cpMomentForPoly(f_mass, NUM_VERTS, verts, cpvzero, 0.0f)));
    f_shape = cpSpaceAddShape(space, cpPolyShapeNew(f_body, NUM_VERTS, verts, cpTransformIdentity, f_size));
    
    cpBodySetPosition(f_body, cpv(rnd(WIDTH), rnd(HEIGHT)));    
    cpShapeSetElasticity(f_shape, 0.0f);
    cpShapeSetFriction(f_shape, 0.7f);
    cpShapeSetCollisionType(f_shape, FOOD);
    
    return food;
}

void foodFree(food_t* food){
    free(food);
}

