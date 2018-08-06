#pragma once

#include "utils.h"

typedef struct world_t world_t;
typedef struct {
    v2 p;
    float a;
} food_t;

void food_render(food_t* food, int n);
void food_update(world_t* world);
