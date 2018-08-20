#include <stdlib.h>

#include "brain.h"
#include "brain_shorts.h"

extern void brainAtrInit(brain_t*);
extern void brainAnnInit(brain_t*);
extern void brainSvmInit(brain_t*);

brain_t* 
brainNew(enum brain_type type, int x_cnt, int y_cnt) {

    brain_t* brain = calloc(1, sizeof(brain_t));
    b_xc = x_cnt;
    b_yc = y_cnt;

    switch(type) {
        case BRAIN_ATR: brainAtrInit(brain);
        case BRAIN_SVM: brainSvmInit(brain);
        case BRAIN_ANN: brainAnnInit(brain);
    }

    return brain;
}

void 
brainFree(brain_t* brain) {
     // call innner free 
     b_vt->free(brain);
     // free struct
     free(brain);
}

void 
brainPredict(brain_t* brain, float* data) {
     b_vt->predict(brain, data);
}
