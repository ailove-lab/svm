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
}
