#ifndef ANIMATOR_H
#define ANIMATOR_H

#include "upng.h"
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define NUM_ANIMATION 2
#define NUM_MAX_CLIP 32

typedef struct
{
	upng_t* texture;
} clip_t;

typedef struct
{
	const char* name;
	clip_t clips[NUM_MAX_CLIP];
    int clipCount;
} animation_t;

enum animation_type
{
    state,
    trans,
    loop
};

typedef struct
{
    animation_t *target;
    enum animation_type currentAnimStep;
    enum animation_type currentAnimState;
    int animMatrix[NUM_MAX_CLIP][NUM_MAX_CLIP]; // 2 Kb is too much OwO 
    int animationStepTime;
    int animLastTick;
} animstate_t;


void animateTick();
void loadAnimations();

void setAnimationFlowPlan(animation_t* anim, enum animation_type type, int index);
void registerAnimation(animation_t anim);
void execAnimation(const char* name);
animation_t* getAnimationHandle(const char* name);

#endif
