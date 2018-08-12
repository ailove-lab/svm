#pragma once

#include <chipmunk/chipmunk_private.h>

#include "food.h"

typedef struct ant_t ant_t;

typedef struct world_t {
    cpSpace* space;
    cpArray* ants;
} world_t;

world_t* worldNew(int ants_count, int food_count);
void worldFree(world_t* world);
void worldUpdate(world_t* world);
void worldRender(world_t* world);

