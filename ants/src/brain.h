#pragma once

#include <stdbool.h>

typedef struct svm_parameter svm_parameter;
typedef struct svm_problem   svm_problem;
typedef struct svm_model     svm_model;
typedef struct svm_node      svm_node;

typedef struct brain_t {

    bool trained;

    int x_cnt;
    int y_cnt;
    svm_model**  models; // [y_cnt]

} brain_t;


brain_t* brain_create(int y_cnt, int x_cnt);
void     brain_delete(brain_t* brain);

void brain_train(brain_t* brain, double* data, int size);
void brain_predict(brain_t* brain, double* data);
void brain_load(brain_t* brain, char* filename);
void brain_save(brain_t* brain, char* filename);
