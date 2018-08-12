#include "engine.h"
#include "world.h"

world_t* world;

void update(){
    worldUpdate(world);
}

void render() {
    worldRender(world);
}

int main(int argc, char* argv[]) {
    world = worldNew(15, 15);

    engine_init();
    engine_update = update;
    engine_render = render;    
    engine_start();
    engine_deinit();
    
    worldFree(world);
    return 0;
}
