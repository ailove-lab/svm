#pragma once

#include <chipmunk/chipmunk.h>

#include "utils.h"

typedef struct {
    // physics
    cpFloat  size;
    cpFloat  mass;
    cpBody*  body;
    cpShape* shape;
} box_t;

box_t* boxNew(cpSpace* space);
void boxFree(box_t* box);
