#include "brain.h"

#include "brain_shorts.h"

static void 
load(brain_t* brain, char* filename) {
}

static void 
save(brain_t* brain, char* filename) {
}

static void 
delete(brain_t* brain) {
}

static void
train(brain_t* brain, float* data, int size) {
}

static void 
predict(brain_t* brain, float* data) {
}

static brain_vt vt = {
    .free    = delete,
    .load    = load,
    .save    = save,
    .train   = train,
    .predict = predict,
};

void 
brainAnnInit(brain_t* brain) {
    b_tp = BRAIN_ANN;
    b_vt = &vt;
    b_t  = false;
}
