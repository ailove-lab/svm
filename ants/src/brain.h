#pragma once

#include <stdbool.h>

enum brain_type {BRAIN_ATR, BRAIN_SVM, BRAIN_ANN};

typedef struct brain_t brain_t;
typedef struct brain_vt {
    void (*free   )(brain_t*);
    void (*train  )(brain_t* brain, float* data, int size);
    void (*predict)(brain_t* brain, float* data);
    void (*load   )(brain_t* brain, char* filename);
    void (*save   )(brain_t* brain, char* filename);
} brain_vt;

typedef struct brain_t {
    brain_vt* vt;
    enum brain_type type;
    int x_cnt;
    int y_cnt;
    bool trained;
    void* data;
} brain_t;

brain_t* brainNew(enum brain_type type, int x_cnt, int y_cnt);
void     brainFree(brain_t* brain);
void     brainPredict(brain_t* brain, float* data);
