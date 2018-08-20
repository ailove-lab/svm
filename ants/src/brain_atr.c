// simple attractor brain
// moving forvard positive traits and away from negative 

#include <stdio.h>
#include <math.h>

#include "globals.h"
#include "brain.h"
#include "brain_shorts.h"


static void 
load(brain_t* brain, char* filename) {
    printf("No need to load attractor brain");
}

static void 
save(brain_t* brain, char* filename) {
    printf("No need to save attractor brain");
}

static void 
delete(brain_t* brain) {}

static void
train(brain_t* brain, float* data, int size) {
    printf("No need to train attractor brain");
}

static void 
predict(brain_t* brain, float* data) {
    // vision have two layers
    int vr = VISION_RESOLUTION;
    // center of view field
    int vh = VISION_RESOLUTION>>1;
    
    float x = 0.0; 
    float y = 0.0;
    float v;
    for(int i=0; i<vr; i++){
        // angle
        float a = (float)(i-vh)/(float)(vh)*PI;
        // distractor, move away from obstacles
        v = data[b_yc+i];   // first vsion layer
        x -= v*cos(a)*0.5;
        y -= v*sin(a)*0.5;      
        // attractor, moving forward to food
        v = data[b_yc+vr+i]; // second vision layer
        x += v*cos(a)*1.0;
        y += v*sin(a)*1.0;
    }
    data[0] += (tanh(x)*0.5 - data[0]) * 0.5;
    data[1] += (tanh(y)*0.5 - data[1]) * 0.5;
    data[2] += (atan2(y, x) - data[2]) * 0.5;
}

static brain_vt vt = {
    .free    = delete,
    .load    = load,
    .save    = save,
    .train   = train,
    .predict = predict,
};

void
brainAtrInit(brain_t* brain) {
    b_tp = BRAIN_ATR; 
    b_vt = &vt;
    b_t  = true;
}
