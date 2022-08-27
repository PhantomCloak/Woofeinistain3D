#include "helper.h"

float normalizeAngle(float rayAngle)
{
	rayAngle = remainder(rayAngle, TWO_PI);
	if(rayAngle < 0)
	{
		rayAngle = 	TWO_PI + rayAngle;
	}

	return rayAngle;
}

float distanceBetweenPoints(float x1, float y1, float x2, float y2)
{
	return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}
