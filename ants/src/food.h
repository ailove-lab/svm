#pragma once

#include <chipmunk/chipmunk.h>

#include "utils.h"

typedef struct {
    // physics
    cpFloat  size;
    cpFloat  mass;
    cpBody*  body;
    cpShape* shape;
} food_t;

food_t* foodNew(cpSpace* space);
void foodFree(food_t* food);
