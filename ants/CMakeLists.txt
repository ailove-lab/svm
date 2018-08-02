add_executable(main src/main.c src/utils.c src/world.c src/ant.c src/food.c)
target_link_libraries(main SDL2 m)
