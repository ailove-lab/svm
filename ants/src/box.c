#include <math.h>
#include <stdlib.h>
#include <nanovg.h>

#include "box.h"

#define b_size  (box->size)
#define b_mass  (box->mass)
#define b_body  (box->body)
#define b_shape (box->shape)

extern NVGcontext* vg;

box_t* boxNew(cpSpace* space) {
    box_t* box = calloc(1, sizeof(box_t));

    b_size = 30.0;
    b_mass =  1.0;

    b_body = cpSpaceAddBody(space, cpBodyNew(b_mass, cpMomentForBox(b_mass, b_size, b_size)));
    cpBodySetPosition(b_body, cpv(rnd(WIDTH), rnd(HEIGHT)));    

    b_shape = cpSpaceAddShape(space, cpBoxShapeNew(b_body, b_size, b_size, 0.0));
    cpShapeSetElasticity(b_shape, 0.0f);
    cpShapeSetFriction(b_shape, 0.7f);
    cpShapeSetCollisionType(b_shape, BOX);
    
    return box;
}

void boxFree(box_t* box){
    free(box);
}

