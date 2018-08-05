#pragma once

#define VISION_RESOLUTION 32
#define VISION_RANGE      200.0
#define FEATURES_COUNT    7

#include "utils.h"

typedef struct world_t world_t;

typedef struct ant_t {

    // mass
    float m;
    
    // absolute position
    v2 p;    
    // absotlue normal
    v2 n;
    // absolute angle
    float a;

    double features[VISION_RESOLUTION + FEATURES_COUNT];
} ant_t;

void ants_update(world_t* world);
void ants_render(ant_t* ants, int n);
void ants_vision_render(ant_t* ant, int n);
