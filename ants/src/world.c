#include "world.h"
#include "utils.h"

world_t* world_create(int ants_count, int food_count) {
	world_t* world = calloc(sizeof(world_t), 1);
	// ants
	world->ants = calloc(sizeof(ant_t ), ants_count);
    world->ants_count = ants_count;
    for(int i=0; i<ants_count; i++) {
        ants[i] = (ant_t){.m=10, .p={rnd(WIDTH), rnd(HEIGHT)}, .a=rnd(PI2)};
        ant_update(&ants[i]);
    }
    // food
	world->food = calloc(sizeof(food_t), food_count);
    world->food_count = food_count;	
    for(int i=0; i<food_count; i++) {
        food[i] = (food_t){.p={rnd(WIDTH), rnd(HEIGHT)}, .a=rnd(PI2)};
    }
	return world;
}

void world_delete(world_t* world) {
	free(world->ants);
	free(world->food);
	free(world);
}

void world_update(world_t* world) {
    ants_update(world);
}

void world_render(world_t* world) {
    ants_render(world->ants, world->ants_count);
    food_render(world->food, world->food_count);
    ants_vision_render(world->ants, world->ants_count);
}

