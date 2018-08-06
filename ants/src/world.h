#pragma once

// #include "ant.h"
#include "food.h"

typedef struct ant_t ant_t;

typedef struct world_t {
    ant_t** ants;
    food_t* food;
    int food_count;
    int ants_count;
} world_t;

world_t* world_create(int ants_count, int food_count);
void world_delete(world_t* world);

void world_update(world_t* world);
void world_render(world_t* world);

