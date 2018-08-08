#include "engine.h"
#include "world.h"

world_t* world;
void update(){
    world_update(world);
}

void render() {
    world_render(world);
}

int main(int argc, char* argv[]) {
    world = world_create(5, 5);
    engine_init();
    engine_update = update;
    engine_render = render;    
    engine_start();
    engine_deinit();
    world_delete(world);
    return 0;
}
