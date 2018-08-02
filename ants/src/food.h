#pragma once

#include "utils.h"

typedef struct {
    v2 p;
    float a;
} food_t;

void food_render(food_t* food, int n);

