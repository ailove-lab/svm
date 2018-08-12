#include <stdlib.h>

#include "world.h"
#include "ant.h"
#include "utils.h"

#define w_space (world->space)
#define w_ants  (world->ants)

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
    world->space = cpSpaceNew();
    
    w_ants = cpArrayNew(ants_count);   

    for(int i=0; i<ants_count; i++) {
        cpArrayPush(w_ants, antNew(world));
    }

    // walls
	cpBody *staticBody = cpSpaceGetStaticBody(w_space);
	cpShape *shape;
	for(int i=0; i<50; i++){
		cpBody *body = add_box(w_space, 20, 1.0);
		
		cpConstraint *pivot = cpSpaceAddConstraint(w_space, cpPivotJointNew2(staticBody, body, cpvzero, cpvzero));
		cpConstraintSetMaxBias(pivot, 0); // disable joint correction
		cpConstraintSetMaxForce(pivot, 1.0f); // emulate linear friction

		cpConstraint *gear = cpSpaceAddConstraint(w_space, cpGearJointNew(staticBody, body, 0.0f, 1.0f));
		cpConstraintSetMaxBias(gear, 0); // disable joint correction
		cpConstraintSetMaxForce(gear, 5.0f); // emulate angular friction
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

void worldUpdate(world_t* world) {
    cpSpaceStep(w_space, 1.0/60.0);
    antsUpdate(world);
}


extern void DrawSegment(
    cpVect a, 
    cpVect b, 
    cpSpaceDebugColor color, 
    cpDataPointer data);

static inline cpSpaceDebugColor RGBAColor(float r, float g, float b, float a){
	cpSpaceDebugColor color = {r, g, b, a};
	return color;
}

void worldRender(world_t* world) {
    spaceDraw(w_space);

    ant_t* a = world->ants->arr[0];
    cpVect start = a->body->p; 
	cpSegmentQueryInfo segInfo = {};
	for(float i=0.0;i<PI*2.0; i+=PI/16.0) {
        cpVect start = a->body->p; 
    	cpVect end = start;
        start.x += sin(i)*20.0;
        start.y += cos(i)*20.0;
        end.x += sin(i)*100.0;
        end.y += cos(i)*100.0;
        
    	if(cpSpaceSegmentQueryFirst(w_space, a->body->p, end, 100.0, CP_SHAPE_FILTER_ALL, &segInfo)){
    		cpVect point = segInfo.point;
    		cpVect n = segInfo.normal;
    		// Draw blue over the occluded part of the query
    		DrawSegment(start, point,  RGBAColor(0,0,1,1), NULL);
    		// Draw a little red surface normal
    		DrawSegment(point, cpvadd(point, cpvmult(n, 16)), RGBAColor(1,0,0,1), NULL);
    		// Draw a little red dot on the hit point.
    		// ChipmunkDebugDrawDot(3, point, RGBAColor(1,0,0,1));
    		// ChipmunkDemoPrintString("Segment Query: Dist(%f) Normal(%5.2f, %5.2f)", segInfo.alpha*cpvdist(start, end), n.x, n.y);
    	}
	}
    
}

