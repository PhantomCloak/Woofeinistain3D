#ifndef RAY_H
#define RAY_H
#include <stdbool.h>
#include "constans.h"

struct Ray
{
	float rayAngle;
	float wallHitX;
	float wallHitY;
	float distance;
	bool wasHitVertical;
	int wallHitContent;
} rays[NUM_RAYS];
#endif
