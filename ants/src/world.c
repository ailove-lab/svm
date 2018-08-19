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

    cpSpaceSetDamping(w_space, 0.5);
    
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

ant_t* 
worldGetAnt(world_t* world) {
    return world->ants->arr[world->current_ant_id];
}

void 
worldNextAnt(world_t* world){
    printf("%d\n", world->current_ant_id);
    world->current_ant_id++; 
    if(world->current_ant_id>=w_ants->num) world->current_ant_id-=w_ants->num;
}

void 
worldPrevAnt(world_t* world) {
    world->current_ant_id--;
    if(world->current_ant_id<0) world->current_ant_id+=w_ants->num;
}

void 
worldSwitchAntBrain(world_t* world) {
    antSwitchBrain(worldGetAnt(world));
}

// PRIVATE

static inline cpSpaceDebugColor 
RGBAColor(float r, float g, float b, float a){
    cpSpaceDebugColor color = {r, g, b, a};
    return color;
}

static inline void drawVector(double x, double y, double dx, double dy, NVGcolor c);
void 
worldRender(world_t* world) {
    spaceDraw(w_space);
    
    ant_t* a = worldGetAnt(world);

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

    // local force
    double aa = a->body->a; // ant angle
    double px = a->body->p.x; 
    double py = a->body->p.y; 
    double fx = a->cortex[0];
    double fy = a->cortex[1];
    double dx = fx*cos(aa) - fy*sin(aa);
    double dy = fy*cos(aa) + fx*sin(aa);

    nvgTextAlign(vg, NVG_ALIGN_CENTER | NVG_ALIGN_BOTTOM);
    nvgFillColor(vg, nvgRGBAf(1.0, 1.0, 1.0, 1.0));
    nvgFontSize(vg, 14.0);
    nvgText(vg, px, py-10.0, antTrained(a) ? "SVM" : "ATR", NULL);
     
    drawVector(
        px, py, 
        dx*100.0, dy*100.0,
        nvgRGBAf(0.0, 0.0, 1.0, 1.0));

    if(!antTrained(a)) {    
        // target angle
        dx = cos(aa + a->ta);
        dy = sin(aa + a->ta);
        drawVector( 
            px+dx*20.0, py+dy*20.0,
               dx*10.0,    dy*10.0,
            nvgRGBAf(0.0, 1.0, 0.0, 1.0));
        
        // avoid angle
        dx = cos(aa + a->aa);
        dy = sin(aa + a->aa);
        drawVector( 
            px+dx*20.0, py+dy*20.0,
               dx*10.0,    dy*10.0,
            nvgRGBAf(1.0, 0.0, 0.0, 1.0));
    }
}

static inline void drawVector(double x, double y, double dx, double dy, NVGcolor c) {
    nvgBeginPath(vg);
    nvgMoveTo(vg, x, y);
    nvgLineTo(vg, x+dx, y+dy);
    nvgStrokeColor(vg, c);
    nvgStroke(vg);
}

