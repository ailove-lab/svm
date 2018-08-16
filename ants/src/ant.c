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
#define a_fx     (ant->fx)
#define a_fy     (ant->fy)
#define a_fa     (ant->fa)
#define a_v      (ant->vision)
#define a_vs     (ant->vision_scores)
#define a_m(i)   (ant->memory[a_mid*MEMORY_STEP + (i)])
#define a_mid    (ant->memory_id)

extern NVGcontext* vg;

/// INTERFACE ///

static int id = 0;
ant_t* antNew(world_t* world) {
    ant_t* ant = calloc(1, sizeof(ant_t));

    a_id    = id++;
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
    cpShapeSetFriction(a_shape, 0.7);
    a_shape->filter.categories = 1;

	// cpBody *staticBody = cpSpaceGetStaticBody(world->space);
	// cpConstraint *pivot = cpSpaceAddConstraint(world->space, cpPivotJointNew2(staticBody, a_body, cpvzero, cpvzero));
	// cpConstraintSetMaxBias(pivot, 0); // disable joint correction
	// cpConstraintSetMaxForce(pivot, 10.0f); // emulate linear friction
    // cpConstraint *gear = cpSpaceAddConstraint(world->space, cpGearJointNew(staticBody, a_body, 0.0f, 1.0f));
    // cpConstraintSetMaxBias(gear, 0); // disable joint correction
    // cpConstraintSetMaxForce(gear, 50.0f); // emulate angular friction

    a_brain = brainNew(Y_COUNT, X_COUNT);
    
    return ant;
};

void antFree(ant_t* ant) {
   cpShapeFree(a_shape);
   cpBodyFree(a_body);
   brainFree(a_brain);
   free(ant);
};

void antMove(ant_t* ant, double lx, double ly) {
    // double aa = a_body->a;
    // double fx = lx*cos(aa)-ly*sin(aa); 
    // double fy = ly*cos(aa)+lx*sin(aa);
    cpBodyApplyForceAtLocalPoint(a_body, cpv(lx, ly), cpvzero);
    //cpBodyApplyImpulseAtLocalPoint(a_body, cpv(lx, ly), cpvzero);
};

void antRotate(ant_t* ant, double a){
    cpBodySetTorque(a_body, a);
};

void antPercepetion(ant_t* ant) {
    // Ant vision
    cpFloat aa   = a_body->a;
    cpVect start = a_body->p; 
	cpSegmentQueryInfo segInfo = {};
	int j = 0;
	for(double i=-1.0;i<=1.0; i+=2.0/(double)(VISION_RESOLUTION-1), j++) {
        cpVect start = a_body->p; 
    	cpVect end = start;
    	cpFloat aaa = i*VISION_ANGLE/2.0 + aa;
        start.x += cos(aaa)*10.0;
        start.y += sin(aaa)*10.0;

        end.x += cos(aaa)*VISION_RANGE;
        end.y += sin(aaa)*VISION_RANGE;
        double v = 0.0; 
        cpShape* shape;
    	if(shape = cpSpaceSegmentQueryFirst(a_space, start, end, -1, CP_SHAPE_FILTER_ALL, &segInfo)){
            v = 1.0-cpvdist(start, segInfo.point)/VISION_RANGE;
            if(shape->filter.categories == 1) v *= -1.0;
    	};
    	a_v[j]+=(v-a_v[j])*VISION_DAMPING;
	}
}

static void rememberSitutation(ant_t* ant) {
    // a_m(i) -> ant->memory[MEMORY_STEP*ant->memory_id+i]
    a_m(0) = a_fx;
    a_m(1) = a_fy;
    a_m(2) = a_fa;
    memcpy(&a_m(3), a_v, sizeof(double)*VISION_RESOLUTION);
    a_mid = (a_mid+1) % MEMORY_SIZE;
}

static void collectPositiveExperience(ant_t* ant) {

    // measure surrounding's score
    double score = 0.0;
    for(int j=0; j<VISION_RESOLUTION; j++) {
        score += a_v[j];
    }
    // score decreesing, means we moving away from the objects
    // if (score - a_vs[0] < 1.0) 
    if(a_fx > 0.0) rememberSitutation(ant);

    a_vs[0] = score;
    
}
static void attractor(ant_t* ant) {
   double min = 1000.0;
   double max =-1000.0;
   int min_i = VISION_RESOLUTION/2;
   int max_i = VISION_RESOLUTION/2;
   for(int i=0; i<VISION_RESOLUTION; i++){
       if(min>fabs(a_v[i])) {
          min = fabs(a_v[i]);
          min_i = i;
       }
       if(max<a_v[i]) {
          max = a_v[i];
          max_i = i;
       }
   }
   // rotate to minimum
   a_fa = (double)(min_i-VISION_RESOLUTION/2)/(double)(VISION_RESOLUTION/2);
   //a_fa-= (double)(max_i-VISION_RESOLUTION/2)/(double)(VISION_RESOLUTION/2);
   a_fx = (1.0-a_fa*a_fa)*0.25;
   a_fy = a_fa*0.25;
   a_fa*= 0.5;
   
}

void antThinking(ant_t* ant) {

    if(!a_brain->trained) {
        collectPositiveExperience(ant);
        //if(a_mid == MEMORY_SIZE-1) brainTrain(a_brain, ant->memory, MEMORY_SIZE);
    }

    if(a_brain->trained) {
        brainPredict(a_brain, &a_fx);
        // a_fx = 0.0;
        // a_fy = 0.0;
        // a_fa = 0.0;
        // printf("%f %f %f\n", a_fx, a_fy, a_fa);
    } else {
        attractor(ant);
        //a_fx = rand()%10 ? 0.0 :  1.0;
        //a_fx = rand()%10 ? 0.0 : -0.5;
        //a_fa = rand()%50 ? 0.0 :  1.0;
        //a_fa = rand()%50 ? 0.0 : -1.0;
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
