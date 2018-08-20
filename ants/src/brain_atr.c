#include <stdio.h>
#include <math.h>

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
    /*
    int vr2 = VISION_RESOLUTION/2;
    int j = 1;
    float x = 0.0; 
    float y = 0.0;
    float v;
    float max_v;
    float max_a;
    for(int i=0; i<VISION_RESOLUTION; i++){
        float a = (float)(i-vr2)/(float)(vr2)*VISION_ANGLE/2.0;
        // distractor
        v = a_v[i+0*VISION_RESOLUTION];
        x -= v*cos(a)*0.5;
        y -= v*sin(a)*0.5;
        // attractor
        v = a_v[i+1*VISION_RESOLUTION];
        x += v*cos(a)*1.0;
        y += v*sin(a)*1.0;
        // max value / angle
        if(v > max_v) {
            max_a = a;
            max_v = v;
        }
    }
    ant->ta += (max_a      - ant->ta)*0.5;
    ant->aa += (atan2(y,x) - ant->aa)*0.5;
    // printf("%3.1f %3.1f\n", max_v, max_a);
    // rotate to food
    a_fa = tanh(ant->aa);
    //a_fa-= (float)(max_i-VISION_RESOLUTION/2)/(float)(VISION_RESOLUTION/2);
    a_fx += (tanh(x)*0.5-a_fx)*0.5;
    a_fy += (tanh(y)*0.5-a_fy)*0.5;
    */
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
    b_vt = &vt;
    b_t  = true;
}
