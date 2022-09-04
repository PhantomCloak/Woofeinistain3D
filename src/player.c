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
	int xStart = ((WINDOW_WIDTH / 2) - 32) * 10;
	int xEnd = ((WINDOW_WIDTH / 2) + 32) * 10;
	int yStart = (WINDOW_HEIGHT - 64) * 10;
	int yEnd = (WINDOW_HEIGHT + 64) * 10;

	upng_t* texture = textures[14];
	drawTexture(0,0,0,0,texture);

	drawRectangle(player.x * MINIMAP_SCALE_FACTOR, player.y * MINIMAP_SCALE_FACTOR,
			player.width,
			player.height,0xFFFF0000);
}
