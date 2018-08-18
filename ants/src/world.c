#include <stdlib.h>

#include "nanovg.h"
#include "world.h"
#include "ant.h"
#include "box.h"
#include "utils.h"

#define w_space (world->space)
#define w_ants  (world->ants )
#define w_food  (world->food )
#define w_boxes (world->boxes)

extern  NVGcontext* vg;
extern void spaceDraw(cpSpace* space);

static cpBool onAntCollision(cpArbiter* arb, cpSpace* space, void* data);

world_t* 
worldNew(int ants_count, int food_count) {

    world_t* world = calloc(sizeof(world_t), 1);
    w_space = cpSpaceNew();

    cpSpaceSetDamping(w_space, 0.1);
    
    w_ants  = cpArrayNew(ants_count);   
    w_food  = cpArrayNew(food_count);   
    w_boxes = cpArrayNew(food_count);   

    for(int i=0; i<ants_count; i++) {
        cpArrayPush(w_ants, antNew(world));
    }

    for(int i=0; i<food_count; i++) {
        cpArrayPush(w_food, foodNew(w_space));
    }
    
    for(int i=0; i<food_count; i++) {
        cpArrayPush(w_boxes, boxNew(w_space));
    }
    
    // walls
    { 
        cpShape *shape;
        cpBody *staticBody = cpSpaceGetStaticBody(w_space);

        // Create segments around the edge of the screen.
        shape = cpSpaceAddShape(w_space, cpSegmentShapeNew(staticBody, cpv(0,0), cpv(0, HEIGHT-0), 0.0f));
        cpShapeSetElasticity(shape, 1.0f);
        cpShapeSetFriction  (shape, 1.0f);
        cpShapeSetCollisionType(shape, WALL);

        shape = cpSpaceAddShape(w_space, cpSegmentShapeNew(staticBody, cpv(0,0), cpv(WIDTH-0, 0), 0.0f));
        cpShapeSetElasticity(shape, 1.0f);
        cpShapeSetFriction  (shape, 1.0f);
        cpShapeSetCollisionType(shape, WALL);
        
        shape = cpSpaceAddShape(w_space, cpSegmentShapeNew(staticBody, cpv(WIDTH-0,0), cpv(WIDTH-0, HEIGHT-0), 0.0f));
        cpShapeSetElasticity(shape, 1.0f);
        cpShapeSetFriction  (shape, 1.0f);
        cpShapeSetCollisionType(shape, WALL);

        shape = cpSpaceAddShape(w_space, cpSegmentShapeNew(staticBody, cpv(0, HEIGHT-0), cpv(WIDTH-0, HEIGHT-0), 0.0f));
        cpShapeSetElasticity(shape, 1.0f);
        cpShapeSetFriction  (shape, 1.0f);
        cpShapeSetCollisionType(shape, WALL);
    }

    cpCollisionHandler* h = cpSpaceAddWildcardHandler(w_space, ANT);
    h->beginFunc = onAntCollision; 

    return world;
}

static cpBool 
onAntCollision(cpArbiter* arb, cpSpace* space, void* data) {
    printf("%d x %d\n", arb->a->type, arb->b->type);
    if(arb->b->type == FOOD) {
        arb->b->body->p.x = rnd(WIDTH);
        arb->b->body->p.y = rnd(HEIGHT);
        return cpFalse;
    }
    return cpTrue;
}

void 
worldFree(world_t* world) {

    cpArrayFreeEach(world->ants , free);
    cpArrayFree(world->ants);

    cpArrayFreeEach(world->food , free);
    cpArrayFree(world->food);

    cpArrayFreeEach(world->boxes, free);
    cpArrayFree(world->boxes);
    
    cpSpaceFree(world->space);

    free(world);
}

void 
worldUpdate(world_t* world, float dt) {
    // cpSpaceStep(w_space, dt);
    cpSpaceStep(w_space, 1.0/60.0);
    antsUpdate(world);
}

static inline cpSpaceDebugColor 
RGBAColor(float r, float g, float b, float a){
    cpSpaceDebugColor color = {r, g, b, a};
    return color;
}

void 
worldRender(world_t* world) {
    spaceDraw(w_space);
    ant_t* a = w_ants->arr[0];
    float d = 8.0;
    for(int i=0; i<VISION_LAYERS; i++) {
        for(int j=0; j<VISION_RESOLUTION;  j++) {
            float v = a->vision[j+i*VISION_RESOLUTION];
            nvgBeginPath(vg);
            nvgRect(vg, (d+1)*(j+1), (d+1)*(i+1), d, d);
            if (i==0) nvgFillColor(vg, nvgRGBAf(v, v, v, 1.0));
            else if (v<0.0) nvgFillColor(vg, nvgRGBAf( -v, 0.0, 0.0, 1.0));
            else            nvgFillColor(vg, nvgRGBAf(0.0,   v, 0.0, 1.0));
            nvgFill(vg);
        }
    }
}

