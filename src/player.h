#ifndef PLAYER_H
#define PLAYER_H

#include <math.h>
#include "constans.h"
#include "graphics.h"
#include "vector.h"
#include "mathUtil.h"
#include "textures.h"

typedef struct Player{
	float x;
	float y;
	float width;
	float height;
	int turnDirection;
	int walkDirection;
	float rotationAngle;
	float walkSpeed;
	float turnSpeed;
} player_t;

extern player_t player;
struct Vector2 playerNextPosition(float dt);
void playerSetPosition(int x, int y);
void renderPlayer();

#endif
