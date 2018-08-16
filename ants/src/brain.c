#include <svm.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "brain.h"

#define b_xc (brain->x_cnt)
#define b_yc (brain->y_cnt)
#define b_m  (brain->models)
#define b_t  (brain->trained)

brain_t* brainNew(int y_cnt, int x_cnt) {
    brain_t* brain = calloc(1, sizeof(brain));
    b_xc = x_cnt;
    b_yc = y_cnt;
    b_m  = calloc(y_cnt, sizeof(svm_model*));
    b_t  = false;
    return brain;
}

void brainLoad(brain_t* brain, char* filename) {
    char buf[256];
    for(int i=0; i<b_yc; i++) {
        sprintf(buf, "%s.%d.m", filename, i);
        b_m[i] = svm_load_model(buf);
    }
    b_t = true;
}

void brainSave(brain_t* brain, char* filename) {
    char buf[256];
    for(int i=0; i<b_yc; i++) {
        sprintf(buf, "%s.%d.m", filename, i);
        svm_save_model(buf, b_m[i]);
    }
}

void brainFree(brain_t* brain) {
    // clear models
    for(int i=0; i< b_yc; i++) {
       free(b_m[i]);
    }
    // clear brain
    free(brain);
}


// data
// <---b_yc---> <---b_xc--->
// y0 y1 ... yn x0 x1 ... xm ##
//              ^yc          ^yc+xc
void brainTrain(brain_t* brain, double* data, int size) {
    
    svm_parameter param;
    
	// default values
	param.svm_type    = NU_SVR;
	param.kernel_type = LINEAR;
	param.degree      = 3;
	param.gamma       = 1.0/b_xc;	// 1/num_features
	param.coef0       = 0;

	// training
	param.nu           = 0.5;
	param.cache_size   = 100;
	param.C            = 1;
	param.eps          = 1e-3;
	param.p            = 0.1;
	param.shrinking    = 0;
	param.probability  = 0;
	param.nr_weight    = 0;
	param.weight_label = NULL;
	param.weight       = NULL;
	//cross_validation = 0;

    int data_step = b_yc + b_xc;
    int x_step    = b_xc +    1;

    // for each separate problem y0 .. yn
    for(int i=0; i<b_yc; i++) {
        // fill problems x -> y 
        double y[size];
        svm_node* x[size]; // pointers to x_space
        svm_node  x_space[size*x_step];
        for(int j=0; j<size; j++) {
            y[j] = data[j*data_step+i];
            x[j] = &x_space[j*x_step]; // row start
            for(int k=0; k<b_xc; k++) {
                x_space[j*x_step+k] = (svm_node){k+1, data[j*data_step+b_yc+k]};
            };
            x_space[j*x_step+b_xc] = (svm_node){-1, 0.0}; // end marker
        }
        svm_problem problem = {.l=size, .y=y, .x=x};
    	const char* error_msg = svm_check_parameter(&problem, &param);
    	if(error_msg) {
    		fprintf(stderr,"ERROR: %s\n", error_msg);
    		return;
    	}
        b_m[i] = svm_train(&problem, &param);
    }

    b_t = true;
}


// data
// y0 y1 ... yn x0 x1 .. xn      
void brainPredict(brain_t* brain, double* data) {
    if(!b_t) {
        return;
    };    
    svm_node x[b_xc+1];
    for(int i=0; i<b_xc; i++) x[i] = (svm_node){i, data[b_yc+i]};
    x[b_xc] = (svm_node){-1, 0.0}; // end marker
    for(int i=0; i<b_yc; i++) printf("% 2.1f ", data[i]); printf("-> ");
    for(int i=0; i<b_yc; i++) {
        double p = svm_predict(b_m[i], x);
        data[i] = isnan(p) ? 0.0 : tanh(p);
        // data[i] = tanh(svm_predict(b_m[i], x));
    }    
    for(int i=0; i<b_yc; i++) printf("% 2.1f ", data[i]); printf("\n");
}
