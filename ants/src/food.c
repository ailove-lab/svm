#include <math.h>
#include <stdlib.h>
#include <nanovg.h>

#include "food.h"

#define f_size  (food->size)
#define f_mass  (food->mass)
#define f_body  (food->body)
#define f_shape (food->shape)

extern NVGcontext* vg;

food_t* foodNew(cpSpace* space) {
    food_t* food = calloc(1, sizeof(food_t));

    f_size = 10.0;
    f_mass =  1.0;

    f_body = cpSpaceAddBody(space, cpBodyNew(f_mass, cpMomentForBox(f_mass, f_size, f_size)));
    cpBodySetPosition(f_body, cpv(rnd(WIDTH), rnd(HEIGHT)));    

    f_shape = cpSpaceAddShape(space, cpBoxShapeNew(f_body, f_size, f_size, 0.0));
    cpShapeSetElasticity(f_shape, 0.0f);
    cpShapeSetFriction(f_shape, 0.7f);
    cpShapeSetCollisionType(f_shape, FOOD);
    
    return food;
}

void foodFree(food_t* food){
    free(food);
}

