#pragma once

#include <chipmunk/chipmunk_private.h>

#include "utils.h"

#define VISION_RESOLUTION 32
#define VISION_LAYERS     1
#define VISION_ANGLE      PI
#define VISION_RANGE      200.0
#define Y_COUNT           3
#define X_COUNT           VISION_RESOLUTION 
#define MEMORY_SIZE       1000
#define MEMORY_STEP       (Y_COUNT + X_COUNT)

typedef struct world_t world_t;
typedef struct brain_t brain_t;

typedef struct ant_t {
    int id;
    world_t* world;
    brain_t* brain;
    
    // physics
    cpFloat  size;
    cpFloat  mass;
    cpFloat  moment;
    cpBody*  body;
    cpShape* shape;

    double fx, fy, fa;
    double vision[VISION_RESOLUTION*VISION_LAYERS];
    double vision_scores[2];
    int    memory_id;
    double memory[MEMORY_STEP * MEMORY_SIZE];
} ant_t;

ant_t* antNew();
void antFree   (ant_t* ant);
void antMove   (ant_t* ant, double lx, double ly);
void antRotate (ant_t* ant, double a);
void antsUpdate(world_t* world);
