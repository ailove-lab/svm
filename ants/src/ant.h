#pragma once

#include "utils.h"

typedef struct {
    // mass
    float m;
    // position velocity force
    v2 p, v, f;
    // angle avelocity aforce
    float a, av, af;
    // normal
    v2 n;
    // vision matrix
    float vision[100];
} ant_t;

void ants_update(ant_t* ants, int n);
void ants_render(ant_t* ants, int n);
void ants_vision_render(world_t* world);
