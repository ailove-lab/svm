#pragma once

#include "utils.h"

#define VISION_RESOLUTION 8
#define INTROSPECTION     4
#define Y_COUNT           3
#define X_COUNT           VISION_RESOLUTION
#define VISION_RANGE      200.0
#define MEMORY_SIZE       100
#define MEMORY_STEP       (Y_COUNT + X_COUNT)

typedef struct world_t world_t;
typedef struct brain_t brain_t;

typedef struct ant_t {

    brain_t* brain;
    
    // mass
    double m;
    
    // absolute position
    v2 p;    
    // absotlue normal
    v2 n;
    // absolute angle
    double a;


    // velocity
    v2 v;
    // angular velocity
    double va;
   
    
    double features[MEMORY_STEP];
   
    // storage for best actions

    // score
    double sc;
    double max_dsc;
    double memory[MEMORY_STEP * MEMORY_SIZE];
    int mid;

} ant_t;

ant_t* ant_create();
void ant_delete(ant_t* ant);

void ants_update(world_t* world);
void ants_render(ant_t** ants, int n);
void ants_vision_render(ant_t** ant, int n);
