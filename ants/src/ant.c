#include <nanovg.h>

#include "ant.h"

#include "utils.h"
#include "world.h"
#include "brain.h"
#include "food.h"

#define a_id     ant->id
#define a_world  ant->world
#define a_mass   ant->mass
#define a_radius ant->radius
#define a_moment ant->moment
#define a_body   ant->body
#define a_shape  ant->shape

extern NVGcontext* vg;

/// INTERFACE ///

static int id = 0;
ant_t* antNew(world_t* world) {
    ant_t* ant = calloc(1, sizeof(ant_t));

    a_id = id++;
    a_world = world;
    a_mass   =  1;
    a_radius = 10;
    a_moment = cpMomentForCircle(a_mass, 0, a_radius, cpvzero);
    
    a_body = cpSpaceAddBody(a_world->space, cpBodyNew(a_mass, a_moment));
    cpBodySetPosition(a_body, cpv(rand()%WIDTH, rand()%HEIGHT));

    a_shape = cpSpaceAddShape(a_world->space, cpCircleShapeNew(a_body, a_radius, cpvzero));
    cpShapeSetFriction(a_shape, 0.7);

    return ant;
};

void antFree(ant_t* ant) {
   cpShapeFree(a_shape);
   cpBodyFree(a_body);
   free(ant);
};

void antUpdate(ant_t* ant) {
    cpBodySetForce(a_body, cpv(rand()%21-10, rand()%21-10));
}

void antsUpdate(world_t* world){
   for(int i=0; i<world->ants->num; i++) {
       antUpdate(world->ants->arr[i]);
   } 
}

