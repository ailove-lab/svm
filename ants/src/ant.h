#pragma once

#include <chipmunk/chipmunk_private.h>

#include "utils.h"

#define VISION_RESOLUTION 64
#define VISION_LAYERS     2
#define VISION_ANGLE      PI*2.0
#define VISION_RANGE      500.0
#define VISION_DAMPING    0.5
#define Y_COUNT           3
#define X_COUNT           (VISION_RESOLUTION*VISION_LAYERS)
#define CORTEX_SIZE       (Y_COUNT+X_COUNT)
#define MEMORY_SIZE       5000

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
   
    double  cortex[CORTEX_SIZE];
    double* vision;
    double  vision_scores[2];
    int     memory_id;
    double  memory[CORTEX_SIZE * MEMORY_SIZE];
    double  ta; // target angle
    double  aa; // average angle
} ant_t;

ant_t* antNew();
void antFree   (ant_t* ant);
void antMove   (ant_t* ant, double lx, double ly);
void antRotate (ant_t* ant, double a);
void antsUpdate(world_t* world);
