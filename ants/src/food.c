#include <SDL2/SDL.h>

#include "world.h"
#include "ant.h"
#include "food.h"

extern SDL_Renderer* renderer;

void food_draw(food_t* f) {
    float s = 5.0;
    v2 p = f->p;
    v2 n = {sin(f->a), cos(f->a)}; 
    v2_mul(&n, s);
    SDL_Point points[] = {
       {p.x + n.x, p.y + n.y},
       {p.x + n.y, p.y - n.x},
       {p.x - n.x, p.y - n.y},
       {p.x - n.y, p.y + n.x},
       {p.x + n.x, p.y + n.y},
    };
    SDL_RenderDrawLines(renderer, points, 5);
}

void food_render(food_t* food, int n) {
    SDL_SetRenderDrawColor(renderer, 128, 128, 128, SDL_ALPHA_OPAQUE);
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
