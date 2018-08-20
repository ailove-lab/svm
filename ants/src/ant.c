#include <nanovg.h>

#include "ant.h"

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
#define a_brains (ant->brains)
#define a_bid    (ant->brain_id)
#define a_brain  (ant->brains[a_bid])
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

    a_brains[0] = brainNew(BRAIN_ATR, X_COUNT, Y_COUNT);
    a_brains[1] = brainNew(BRAIN_SVM, X_COUNT, Y_COUNT);
    a_brains[2] = brainNew(BRAIN_ANN, X_COUNT, Y_COUNT);
    a_bid = 0;

    return ant;
};

void 
antFree(ant_t* ant) {
    cpShapeFree(a_shape);
    cpBodyFree(a_body);
    brainFree(a_brains[0]);
    brainFree(a_brains[1]);
    brainFree(a_brains[2]);
    free(ant);
};

bool 
antTrained(ant_t* ant) {
    return a_brain->trained;
}

void 
antSwitchBrain(ant_t* ant) {
    a_bid = (a_bid+1)%3;
}

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
	for(double i=-1.0; i<=1.0; i+=2.0/(double)(VISION_RESOLUTION-1), j++) {

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

void 
antThinking(ant_t* ant) {
    printf("%d - %d\n", a_bid, a_brain->type);
    brainPredict(a_brain, a_c);
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
