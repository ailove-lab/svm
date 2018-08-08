#pragma once

int  engine_init();
void engine_start();
void engine_deinit();

void (*engine_update)();
void (*engine_render)();
