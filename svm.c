#include <stdio.h>

// points, x y + bias
float X[] = {
     0.0,  1.0,
     2.0,  3.0,
     4.0, -1.0,
    -5.0,  4.0,
    -2.0, -1.0,
};

float Y[] = {
   1.0,
   1.0,
  -1.0,
   1.0,
  -1.0,
};

// hiperplane
float W[] = {
     0.0, 0.0, 0.0,
};

int main(){
    
    // learning rate
    float r = 0.01;

    // epochs
    for(int e=1; e<1000000; e++) {
		printf("epoch: %d\tW: [%1.3f %1.3f %1.3f] \n", e, W[0], W[1], W[2]);
		for(int i=0; i<5; i++) {
            // Y[i]*dot(X[i*2], W)
            float h = Y[i]*( X[i*2+0]*W[0] + X[i*2+1]*W[1] + (-1.0)*W[2]);
            if(h<1.0) {
                // w = w + rate * ( (X[i] * Y[i]) + (-2  *(1/epoch)* w) )
                W[0]+= r*(X[i*2+0]*Y[i] - 2.0*W[0]/e);
                W[1]+= r*(X[i*2+1]*Y[i] - 2.0*W[1]/e);
                W[2]+= r*(    -1.0*Y[i] - 2.0*W[2]/e);
            } else {
                // w = w + eta * (-2  *(1/epoch)* w) 
                W[0]-= r*2.0*W[0]/e;
                W[1]-= r*2.0*W[1]/e;
                W[2]-= r*2.0*W[2]/e;
            }
		}
    }
    
    printf("W: [%f %f %f] \n",W[0], W[1], W[2]);
}


