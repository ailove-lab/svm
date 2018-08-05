#include <SDL2/SDL.h>

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

