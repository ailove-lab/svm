#include <stdlib.h>

#include "world.h"
#include "ant.h"
#include "utils.h"

#define w_a  (world->ants)
#define w_f  (world->food)
#define w_ac (world->ants_count)
#define w_fc (world->food_count)

world_t* world_create(int ants_count, int food_count) {
    world_t* world = calloc(sizeof(world_t), 1);
    w_a = calloc(sizeof(ant_t*), ants_count);
    w_f = calloc(sizeof(food_t), food_count);
    w_ac = ants_count;
    w_fc = food_count; 
    for(int i=0; i<w_ac; i++) {
        w_a[i] = ant_create();
    }
    for(int i=0; i<food_count; i++) {
        w_f[i] = (food_t){.p={rnd(WIDTH), rnd(HEIGHT)}, .a=rnd(PI2)};
    }
    world_update(world);
    return world;
}

void world_delete(world_t* world) {
    for(int i=0; i<w_ac; i++) ant_delete(w_a[i]);
    free(w_a);
    free(w_f);
    free(world);
}

void world_update(world_t* world) {
    ants_update(world);
    food_update(world);
}

void world_render(world_t* world) {
    ants_render(w_a, w_ac);
    food_render(w_f, w_fc);
    ants_vision_render(w_a, w_ac);
}

