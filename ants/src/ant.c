#include <nanovg.h>

#include "ant.h"

#include "utils.h"
#include "world.h"
#include "brain.h"
#include "food.h"

#define a_id     (ant->id)
#define a_world  (ant->world)
#define a_space  (ant->world->space)
#define a_mass   (ant->mass)
#define a_size   (ant->size)
#define a_moment (ant->moment)
#define a_body   (ant->body)
#define a_p      (ant->body->p)
#define a_shape  (ant->shape)
#define a_brain  (ant->brain)
#define a_c      (ant->cortex)
#define a_fx     (a_c[0])
#define a_fy     (a_c[1])
#define a_fa     (a_c[2])
#define a_v      (ant->vision)
#define a_vs     (ant->vision_scores)
#define a_m(i)   (ant->memory[a_mid*CORTEX_SIZE + (i)])
#define a_mid    (ant->memory_id)

extern NVGcontext* vg;

/// INTERFACE ///

static int id = 0;

ant_t* 
antNew(world_t* world) {
    ant_t* ant = calloc(1, sizeof(ant_t));

    a_id    = id++;
    a_v     = &a_c[Y_COUNT];
    a_world = world;
    a_mass  =  1;
    a_size  = 10;
    a_fx = a_fy = a_fa = 0.0;

    a_mid = 0;
    
    a_moment = cpMomentForCircle(a_mass, 0, a_size, cpvzero);
    
    a_body = cpSpaceAddBody(a_world->space, cpBodyNew(a_mass, a_moment));
    cpBodySetPosition(a_body, cpv(rand()%WIDTH, rand()%HEIGHT));
    cpBodySetAngle(a_body, (double)rand()/(double)RAND_MAX*PI*2.0);

    a_shape = cpSpaceAddShape(a_world->space, cpCircleShapeNew(a_body, a_size, cpvzero));
    cpShapeSetFriction(a_shape, 0.1);
    a_shape->filter.categories = 1;
    cpShapeSetUserData(a_shape, ant);
    cpShapeSetCollisionType(a_shape, ANT);

    a_brain = brainNew(Y_COUNT, X_COUNT);
    
    return ant;
};

void 
antFree(ant_t* ant) {
   cpShapeFree(a_shape);
   cpBodyFree(a_body);
   brainFree(a_brain);
   free(ant);
};

void 
antMove(ant_t* ant, double lx, double ly) {
    cpBodyApplyForceAtLocalPoint(a_body, cpv(lx, ly), cpvzero);
};

void 
antRotate(ant_t* ant, double a){
    cpBodySetTorque(a_body, a);
};

void
antPercepetion(ant_t* ant) {
    // Ant vision
    cpFloat aa   = a_body->a;
    cpVect start = a_body->p;

    // avoiding self ray-query by temporary disabling collision 
    cpShapeFilter f = a_shape->filter;
    a_shape->filter = CP_SHAPE_FILTER_NONE;
    
	cpSegmentQueryInfo segInfo = {};
	int j = 0;
	for(double i=-1.0;i<=1.0; i+=2.0/(double)(VISION_RESOLUTION-1), j++) {

        cpVect start = a_body->p; 
    	cpVect end = start;
    	cpFloat aaa = i*VISION_ANGLE/2.0 + aa;

        end.x += cos(aaa)*VISION_RANGE;
        end.y += sin(aaa)*VISION_RANGE;
        double v0 = 0.0;
        double v1 = 0.0;

        cpShape* shape = cpSpaceSegmentQueryFirst(a_space, start, end, -1.0, CP_SHAPE_FILTER_ALL, &segInfo);

    	if(shape) {
            v0 = 1.0-cpvdist(start, segInfo.point)/VISION_RANGE;
            if(shape->type == FOOD) v1 = v0;
            if(shape->type == ANT ) v1 =-v0;
            v0 =pow(v0, 4.0);
    	};
    	a_v[0*VISION_RESOLUTION+j]+=(v0-a_v[0*VISION_RESOLUTION+j])*VISION_DAMPING;
    	a_v[1*VISION_RESOLUTION+j]+=(v1-a_v[1*VISION_RESOLUTION+j])*VISION_DAMPING;
	}
	// enable collision back
    a_shape->filter = f;
}

static void 
rememberSitutation(ant_t* ant) {
    // a_m >> ant_memory[a_mid*CORTEX_SIZE+0]
    memcpy(&a_m(0), a_c, sizeof(double)*CORTEX_SIZE);
    a_mid = (a_mid+1) % MEMORY_SIZE;
}

static void 
onBBQuery(cpShape* shape, void* data) {
    *(double*)data+= 1.0;
}

static void 
collectPositiveExperience(ant_t* ant) {

    // measure surrounding's score
    double score = 0.0;
    // cpSpaceBBQuery(a_space, cpBBNewForCircle(a_body->p, 100.0), CP_SHAPE_FILTER_ALL, onBBQuery, &score);
    for(int j=0; j<VISION_RESOLUTION; j++) {
        score += a_v[j+VISION_RESOLUTION];
    }
    // score decreesing, means we moving away from the objects
    if(score - a_vs[0] < 0.0) 
        rememberSitutation(ant);

    a_vs[0] = score;
}

static void 
attractor(ant_t* ant) {
   int vr2 = VISION_RESOLUTION/2;
   int j = 1;
   double x = 0.0; 
   double y = 0.0;
   double v;
   double max_v;
   double max_a;
   for(int i=0; i<VISION_RESOLUTION; i++){
       double a = (double)(i-vr2)/(double)(vr2)*VISION_ANGLE/2.0;

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
   //a_fa-= (double)(max_i-VISION_RESOLUTION/2)/(double)(VISION_RESOLUTION/2);
   a_fx += (tanh(x)*0.5-a_fx)*0.5;
   a_fy += (tanh(y)*0.5-a_fy)*0.5;
   
}

void 
antThinking(ant_t* ant) {

    if(!a_brain->trained) {
        collectPositiveExperience(ant);
        // memory filled, train brain
        if(a_mid == MEMORY_SIZE-1) {
            brainTrain(a_brain, ant->memory, MEMORY_SIZE);
            char name[32];
            sprintf(name, "brain.%0d", a_id);
            brainSave(a_brain, name);
        }
    }

    if(a_brain->trained) {
        brainPredict(a_brain, &a_fx);
    } else {
        attractor(ant);
    }
}

void antAction(ant_t* ant) {
    cpBodyApplyForceAtLocalPoint(a_body, cpv(a_fx*1000.0, a_fy*1000.0), cpvzero);
    cpBodySetTorque(a_body, a_fa*1000.0);
}

// perception -> thinking -> action
void antUpdate(ant_t* ant) { 
    antPercepetion(ant);
    antThinking(ant);
    antAction(ant);
    if(a_p.x>WIDTH || a_p.x<0.0) a_p.x = rand()%WIDTH;
    if(a_p.y>WIDTH || a_p.y<0.0) a_p.y = rand()%HEIGHT;
}

void antsUpdate(world_t* world){
   for(int i=0; i<world->ants->num; i++) {
       antUpdate(world->ants->arr[i]);
   }
}
