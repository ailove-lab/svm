#include <GLFW/glfw3.h>

#include "engine.h"
#include "world.h"
#include "ant.h"

world_t* world;

void update(float dt){
    worldUpdate(world, dt);
}

void render(float dt) {
    worldRender(world);
}

void key(int key, int action) {
    ant_t* a = world->ants->arr[0];    
	if (key == GLFW_KEY_KP_8) a->fx = !action ? 0.0 : 1.0;
	if (key == GLFW_KEY_KP_5) a->fx = !action ? 0.0 :-1.0;
	if (key == GLFW_KEY_KP_4) a->fa = !action ? 0.0 :-1.0;
	if (key == GLFW_KEY_KP_6) a->fa = !action ? 0.0 : 1.0;
}

int main(int argc, char* argv[]) {
    world = worldNew(5, 50);

    engine_init();
    engine_update = update;
    engine_render = render; 
    engine_key    = key; 
    engine_start();
    engine_deinit();
    
    worldFree(world);
    return 0;
}


