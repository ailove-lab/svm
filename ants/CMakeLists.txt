cmake_minimum_required(VERSION 3.11)

project(svm_ants)

find_package(glfw3 3.2 REQUIRED)
find_package(OpenGL REQUIRED)

find_package(GLEW REQUIRED)
set(CMAKE_C_FLAGS "-g -O0")

add_definitions(-DNANOVG_GL2 -DNANOVG_GLEW)
include_directories(libsvm)
include_directories(nanovg/src/)
include_directories(${OPENGL_INCLUDE_DIR})
include_directories(${GLEW_INCLUDE_DIRS})

add_library(nanovg STATIC 
    nanovg/src/nanovg.c
) 

add_executable(main 
    src/main.c
    src/engine.c
    src/utils.c 
    src/world.c 
    src/ant.c 
    src/brain.c 
    src/food.c
    libsvm/svm.cpp)
target_link_libraries(main nanovg ${OPENGL_gl_LIBRARY} ${GLEW_LIBRARIES} GL glfw m)
