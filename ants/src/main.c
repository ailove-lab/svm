
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
} ant_t;

typedef struct {
    v2 p;
    float a;
} food_t;

void v2_mul(v2* a, float k) {
    a->x*=k;
    a->y*=k;
}

void ant_update(ant_t* a) {
    
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

void ant_draw(ant_t* a) {
    float s = 10.0;
    v2 p = a->p;
    v2 n = a->n;
    v2_mul(&n, s);
    v2 t = {n.y, -n.x}; // perpendiuclar to normal
    SDL_Point points[] = {
       {p.x + n.x, p.y + n.y},
       {p.x - n.x - t.x/2.0, p.y - n.y-t.y/2.0},
       {p.x - n.x + t.x/2.0, p.y - n.y+t.y/2.0}, 
       {p.x + n.x, p.y + n.y},
    };
    SDL_RenderDrawLines(renderer, points, 4);
}

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

void ant_kick(ant_t* a) {
    if (rand()%100>95) a->f = a->n;
    else a->f = (v2){0.0, 0.0}; 
    
    if (rand()%100>95) a->af = (50-rand()%100)/500.0;
    else a->af = 0.0; 
}

float rnd(float r) {return rand()%1000/1000.0*r;}

void ants_render(ant_t* ants) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);

    for(int i=0; i<10; i++){
        ant_kick  (&ants[i]);
        ant_update(&ants[i]);
        ant_draw  (&ants[i]);
    }
}

void generate_ants(ant_t* ants, int cnt) {
    for(int i=0; i<cnt; i++) {
        ants[i] = (ant_t){.m=10, .p={rnd(WIDTH), rnd(HEIGHT)}, .a=rnd(PI2)};
        ant_update(&ants[i]);
    }
}

void generate_food(food_t* food, int cnt) {
    for(int i=0; i<cnt; i++) {
        food[i] = (food_t){.p={rnd(WIDTH), rnd(HEIGHT)}, .a=rnd(PI2)};
    }
}

void food_render(food_t* food) {
    SDL_SetRenderDrawColor(renderer, 128, 128, 128, SDL_ALPHA_OPAQUE);

    for(int i=0; i<10; i++){
        food_draw(&food[i]);
    }
}

void render() {
    ants_render(ants);
    food_render(food);
}

int main(int argc, char* argv[]) {

    ant_t  ants[10];
    food_t food[10];
    
    generate_ants(ants, 10);
    generate_food(food, 10);

    if (SDL_Init(SDL_INIT_VIDEO) == 0) {

        if (SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, 0, &window, &renderer) == 0) {
            SDL_bool done = SDL_FALSE;

            while (!done) {
                SDL_Event event;

                SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
                SDL_RenderClear(renderer);

                
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
    return 0;
}
