
#include <SDL2/SDL.h>

#include "world.h"

SDL_Window*   window   = NULL;
SDL_Renderer* renderer = NULL;

int main(int argc, char* argv[]) {

    world_t* world = world_create(5, 5);

    if (SDL_Init(SDL_INIT_VIDEO) == 0) {

        if (SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, 0, &window, &renderer) == 0) {
            SDL_bool done = SDL_FALSE;

            while (!done) {
                SDL_Event event;

                SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
                SDL_RenderClear(renderer);

                world_update(world);
                world_render(world);

                SDL_RenderPresent(renderer);
                
                while (SDL_PollEvent(&event)) {
                    if (event.type == SDL_QUIT) {
                        done = SDL_TRUE;
                    }
                }
            }
        }

        if (renderer) {
            SDL_DestroyRenderer(renderer);
        }
        if (window) {
            SDL_DestroyWindow(window);
        }
    }
    SDL_Quit();
    
    world_delete(world);
    return 0;
}
