#include <math.h>
#include <stdlib.h>
#include <nanovg.h>

#include "world.h"
#include "ant.h"
#include "food.h"

extern NVGcontext* vg;

void food_draw(food_t* f) {   
    nvgBeginPath(vg);
    nvgCircle(vg, f->p.x, f->p.y, 5.0);
    nvgFillColor(vg, nvgRGBA(128,128, 128, 255));
    nvgFill(vg);
}

void food_render(food_t* food, int n) {
    for(int i=0; i<n; i++){
        food_draw(&food[i]);
    }
}

void food_update(world_t* world) {
    for(int i=0; i<world->food_count; i++) {
        for(int j=0; j<world->ants_count; j++) {
            v2* p0 = &world->food[i].p;
            v2* p1 = &world->ants[j]->p;
            v2 pp = {p0->x - p1->x, p0->y - p1->y};
            double l = v2_len(&pp);
            if(l<20.0) {
                p0->x = rand()%WIDTH;
                p0->y = rand()%HEIGHT;
            } 
        }
    }
}
