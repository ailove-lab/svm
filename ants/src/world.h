#pragma once

#include <chipmunk/chipmunk_private.h>

#include "food.h"

typedef struct ant_t ant_t;

typedef struct world_t {
    cpSpace* space;
    cpArray* ants;
    cpArray* food;
    cpArray* boxes;

    int current_ant_id;
} world_t;

world_t* worldNew(int ants_count, int food_count);
void   worldFree   (world_t* world);
void   worldUpdate (world_t* world, float dt);
void   worldRender (world_t* world);
ant_t* worldGetAnt (world_t* world);
void   worldSelectNextAnt(world_t* world);
void   worldSelectPrevAnt(world_t* world);
void   worldSwitchSelectedAntBrain(world_t* world);
