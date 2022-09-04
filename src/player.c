#include "player.h"

player_t player = {
	.x = WINDOW_WIDTH / 1.8,
	.y = WINDOW_HEIGHT / 1.8,
	.width = 5,
	.height = 5,
	.turnDirection = 0,
	.walkDirection = 0,
	.rotationAngle = PI / 2,
	.walkSpeed = 250,
	.turnSpeed = 85 * (PI / 180)
};

struct Vector2 playerNextPosition(float dt)
{
	player.rotationAngle += player.turnDirection * player.turnSpeed * dt;
	player.rotationAngle = normalizeAngle(player.rotationAngle);
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


void renderPlayer()
{
	drawRectangle(player.x * MINIMAP_SCALE_FACTOR, player.y * MINIMAP_SCALE_FACTOR,
			player.width,
			player.height,0xFFFF0000);
}
