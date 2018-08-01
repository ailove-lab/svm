
#include <SDL2/SDL.h>

#define PI  3.1415926
#define PI2 PI*2.0
#define WIDTH  800
#define HEIGHT 800

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

typedef struct {
    float x, y;
} v2;

float friction = 0.95;
typedef struct {
    // mass
    float m;
    // position velocity force
    v2 p, v, f;
    // angle avelocity aforce
    float a, av, af;

    // normal
    v2 n;    
} ant;

void v2_mul(v2* a, float k) {
    a->x*=k;
    a->y*=k;
}

void ant_update(ant* a) {
    
    // linear motion
    a->v.x+= a->f.x/a->m;
    a->v.y+= a->f.y/a->m;
    a->v.x*= friction; 
    a->v.y*= friction;
    a->p.x+= a->v.x;
    a->p.y+= a->v.y;
    if(a->p.x > WIDTH ) a->p.x-=WIDTH;
    if(a->p.y > HEIGHT) a->p.y-=HEIGHT;
    if(a->p.x < 0) a->p.x+=WIDTH;
    if(a->p.y < 0) a->p.y+=HEIGHT;

    // rotation
    a->av+= a->af/a->m;
    a->av*= friction;
    a->a += a->av;
    if(a->a > PI2) a->a-=PI2;
    if(a->a < 0  ) a->a+=PI2;

    // normal
    a->n.x = sin(a->a);
    a->n.y = cos(a->a);
}

void ant_draw(ant* a) {
    float s = 20.0;
    v2 p = a->p;
    v2 n = a->n;
    v2_mul(&n, 10.0);
    v2 t = {n.y, -n.x}; // perpendiuclar to normal
    SDL_Point points[] = {
       {p.x + n.x, p.y + n.y},
       {p.x - n.x - t.x/2.0, p.y - n.y-t.y/2.0},
       {p.x - n.x + t.x/2.0, p.y - n.y+t.y/2.0}, 
       {p.x + n.x, p.y + n.y},
    };
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawLines(renderer, points, 4);
    SDL_RenderPresent(renderer);
}


int main(int argc, char* argv[]) {

    ant a={.m=10, .p={100.0, 100.0}, .a=0.0};
    ant_update(&a);

    if (SDL_Init(SDL_INIT_VIDEO) == 0) {

        if (SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, 0, &window, &renderer) == 0) {
            SDL_bool done = SDL_FALSE;

            while (!done) {
                SDL_Event event;

                SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
                SDL_RenderClear(renderer);

                if (rand()%100>95) a.f = a.n;
                else a.f = (v2){0.0, 0.0}; 
                
                if (rand()%100>95) a.af = (50-rand()%100)/500.0;
                else a.af = 0.0; 

                ant_update(&a);
                ant_draw(&a);

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
    return 0;
}
