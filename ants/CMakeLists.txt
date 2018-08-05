set(CMAKE_C_FLAGS "-g -O0")
include_directories(libsvm)
add_executable(main 
    src/main.c 
    src/utils.c 
    src/world.c 
    src/ant.c 
    src/brain.c 
    src/food.c 
    libsvm/svm.cpp)
target_link_libraries(main SDL2 m)
