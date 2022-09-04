#ifndef RAY_H
#define RAY_H

#include <stdbool.h>
#include "player.h"
#include "map.h"
#include "constans.h"
#include "mathUtil.h"

typedef struct
{
	float rayAngle;
	float wallHitX;
	float wallHitY;
	float distance;
	bool wasHitVertical;
	int wallHitContent;
} ray_t;
	
typedef enum
{
	RAY_FACING_UP,
	RAY_FACING_DOWN,
	RAY_FACING_RIGHT,
	RAY_FACING_LEFT
} direction_t;

extern ray_t rays[NUM_RAYS];

void castRay(float rayAngle, int index);

direction_t getRayDirectionVertical(ray_t* ray);
direction_t getRayDirectionHorizontal(ray_t* ray);

void castAllRays();
void renderRays();
#endif
