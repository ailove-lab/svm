#include <stdlib.h>

#include "nanovg.h"
#include "world.h"
#include "ant.h"
#include "utils.h"

#define w_space (world->space)
#define w_ants  (world->ants)

extern  NVGcontext* vg;
extern void spaceDraw(cpSpace* space);

static inline cpFloat
frand(void) {
	return (cpFloat)rand()/(cpFloat)RAND_MAX;
}
static cpBody *
add_box(cpSpace *space, cpFloat size, cpFloat mass) {
	cpFloat radius = cpvlength(cpv(size, size));

	cpBody *body = cpSpaceAddBody(space, cpBodyNew(mass, cpMomentForBox(mass, size, size)));
	cpBodySetPosition(body, cpv(frand()*WIDTH, frand()*HEIGHT));	

	cpShape *shape = cpSpaceAddShape(space, cpBoxShapeNew(body, size, size, 0.0));
	cpShapeSetElasticity(shape, 0.0f);
	cpShapeSetFriction(shape, 0.7f);
	
	return body;
}

world_t* worldNew(int ants_count, int food_count) {

    world_t* world = calloc(sizeof(world_t), 1);
    w_space = cpSpaceNew();

    cpSpaceSetDamping(w_space, 0.1);
    
    w_ants = cpArrayNew(ants_count);   

    for(int i=0; i<ants_count; i++) {
        cpArrayPush(w_ants, antNew(world));
    }

    // walls
	cpBody *staticBody = cpSpaceGetStaticBody(w_space);
	cpShape *shape;
	for(int i=0; i<food_count; i++){
		cpBody *body = add_box(w_space, 20, 1.0);
		
		// cpConstraint *pivot = cpSpaceAddConstraint(w_space, cpPivotJointNew2(staticBody, body, cpvzero, cpvzero));
		// cpConstraintSetMaxBias(pivot, 0); // disable joint correction
		// cpConstraintSetMaxForce(pivot, 10.0f); // emulate linear friction

		// cpConstraint *gear = cpSpaceAddConstraint(w_space, cpGearJointNew(staticBody, body, 0.0f, 1.0f));
		// cpConstraintSetMaxBias(gear, 0); // disable joint correction
		// cpConstraintSetMaxForce(gear, 50.0f); // emulate angular friction
	}
    
		
	// Create segments around the edge of the screen.
	shape = cpSpaceAddShape(w_space, cpSegmentShapeNew(staticBody, cpv(0,0), cpv(0, HEIGHT-0), 0.0f));
	cpShapeSetElasticity(shape, 1.0f);
	cpShapeSetFriction  (shape, 1.0f);
	// cpShapeSetFilter    (shape, NOT_GRABBABLE_FILTER);

    shape = cpSpaceAddShape(w_space, cpSegmentShapeNew(staticBody, cpv(0,0), cpv(WIDTH-0, 0), 0.0f));
	cpShapeSetElasticity(shape, 1.0f);
	cpShapeSetFriction  (shape, 1.0f);
	
    shape = cpSpaceAddShape(w_space, cpSegmentShapeNew(staticBody, cpv(WIDTH-0,0), cpv(WIDTH-0, HEIGHT-0), 0.0f));
	cpShapeSetElasticity(shape, 1.0f);
	cpShapeSetFriction  (shape, 1.0f);

    shape = cpSpaceAddShape(w_space, cpSegmentShapeNew(staticBody, cpv(0, HEIGHT-0), cpv(WIDTH-0, HEIGHT-0), 0.0f));
	cpShapeSetElasticity(shape, 1.0f);
	cpShapeSetFriction  (shape, 1.0f);

    return world;
}

void worldFree(world_t* world) {
    cpArrayFreeEach(world->ants, free);
    cpArrayFree(world->ants);
    cpSpaceFree(world->space);
    free(world);
}

void worldUpdate(world_t* world, float dt) {
    cpSpaceStep(w_space, dt);
    antsUpdate(world);
}

static inline cpSpaceDebugColor RGBAColor(float r, float g, float b, float a){
	cpSpaceDebugColor color = {r, g, b, a};
	return color;
}

void worldRender(world_t* world) {
    spaceDraw(w_space);
    ant_t* a = w_ants->arr[0];
    float d = 8.0; 
    for(int j=0; j<VISION_RESOLUTION;  j++) {
        float v = a->vision[j];
        nvgBeginPath(vg);
        nvgRect(vg, (d+1)*(j+1), d, d, d);
        if (v<0.0) nvgFillColor(vg, nvgRGBAf(-v, 0.0, 0.0, 1.0));
        else nvgFillColor(vg, nvgRGBAf(v, v, v, 1.0));
        nvgFill(vg);
    }
}

