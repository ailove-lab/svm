
#include <SDL2/SDL.h>

#include "world.h"


#define WIDTH  800
#define HEIGHT 800

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

/*

ant perception

   velocity
   x,y

   objects around

      landscape[]

      food[]
         direction
         distnace

      ants[]
         direction
         distnace

*/



int main(int argc, char* argv[]) {

    world_t* world = world_create(10,10);


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
