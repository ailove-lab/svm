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

brain_t* brainNew (int y_cnt, int x_cnt);
void     brainFree(brain_t* brain);

void brainTrain  (brain_t* brain, double* data, int size);
void brainPredict(brain_t* brain, double* data);
void brainLoad   (brain_t* brain, char* filename);
void brainSave   (brain_t* brain, char* filename);
