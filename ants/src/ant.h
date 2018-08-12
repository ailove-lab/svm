#pragma once

#include <chipmunk/chipmunk_private.h>

#include "utils.h"

#define VISION_RESOLUTION 16
#define INTROSPECTION     4
#define Y_COUNT           3
#define X_COUNT           VISION_RESOLUTION
#define VISION_RANGE      200.0
#define MEMORY_SIZE       1000
#define MEMORY_STEP       (Y_COUNT + X_COUNT)

typedef struct world_t world_t;
typedef struct brain_t brain_t;

typedef struct ant_t {
    int id;
    world_t* world;
    cpFloat radius;
    cpFloat mass;
    cpFloat moment;
    cpBody* body;
    cpShape* shape;
    
} ant_t;

ant_t* antNew();
void antFree(ant_t* ant);
void antsUpdate(world_t* world);
