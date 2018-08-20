#pragma once

#include <stdbool.h>
#include <chipmunk/chipmunk_private.h>

#include "globals.h"
#include "utils.h"

typedef struct world_t world_t;
typedef struct brain_t brain_t;

typedef struct ant_t {
    int id;

    world_t* world;
    brain_t* brains[3];
    int      brain_id;

    // physics
    cpFloat  size;
    cpFloat  mass;
    cpFloat  moment;
    cpBody*  body;
    cpShape* shape;
   
    float  cortex[CORTEX_SIZE];
    float* vision;
    float  vision_scores[2];
    int    memory_id;
    float  memory[CORTEX_SIZE * MEMORY_SIZE];
    float  ta; // target angle
    float  aa; // average angle
} ant_t;

ant_t* antNew();

void antFree   (ant_t* ant);
bool antTrained(ant_t* ant);
void antSwitchBrain(ant_t* ant);
void antsUpdate(world_t* world);
