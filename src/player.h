#include <math.h>
#include "vector.h"

struct Player{
	float x;
	float y;
	float width;
	float height;
	int turnDirection;
	int walkDirection;
	float rotationAngle;
	float walkSpeed;
	float turnSpeed;
} player;

extern struct Vector2 playerNextPosition(float dt);
extern void playerSetPosition(int x, int y);

