#include "player.h"


struct Vector2 playerNextPosition(float dt)
{
	player.rotationAngle += player.turnDirection * player.turnSpeed * dt;
	float moveStep = player.walkDirection * player.walkSpeed * dt;

	float newPlayerX = player.x + cos(player.rotationAngle) * moveStep;
	float newPlayerY = player.y + sin(player.rotationAngle) * moveStep;
	
	struct Vector2 nextPos;
	nextPos.x = newPlayerX;
	nextPos.y = newPlayerY;

	return nextPos;
}

void playerSetPosition(int x, int y)
{
    player.x = x;
    player.y = y;
}


