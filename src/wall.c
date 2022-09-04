#include "wall.h"

void renderWalls()
{
	for(int x = 0; x < NUM_RAYS; x++)
	{
		float wallDistance = rays[x].distance * cos(rays[x].rayAngle - player.rotationAngle);
		
		float wallHeight = (TILE_SIZE / wallDistance) * DIST_PROJ_PLANE;

		int wallTopY = (WINDOW_HEIGHT / 2)  - (wallHeight / 2);
		wallTopY = wallTopY < 0 ? 0 : wallTopY;

		int wallBotY = ((float)WINDOW_HEIGHT / 2)  + (wallHeight / 2);
		wallBotY = wallBotY > WINDOW_HEIGHT ? WINDOW_HEIGHT : wallBotY;
		
        // set the color of the ceiling
        for (int y = 0; y < wallTopY; y++) {
            drawPixel(x, y, 0xFF444444);
        }

		int textureOffsetX;
		if(rays[x].wasHitVertical)
			textureOffsetX = (int)rays[x].wallHitY % TILE_SIZE;
		else
			textureOffsetX = (int)rays[x].wallHitX % TILE_SIZE;

		int texNum = rays[x].wallHitContent - 1;

		upng_t* wallTexture = textures[texNum];
		int wallTextureWidth = upng_get_width(wallTexture);
		int wallTextureHeight = upng_get_height(wallTexture);
		uint32_t* wallTextureBuffer = (uint32_t*)upng_get_buffer(wallTexture);

		for(int y = wallTopY; y < wallBotY; y++)
		{
			int distanceFromTop = y + (wallHeight / 2) - (WINDOW_HEIGHT / 2);
			int textureOffsetY = distanceFromTop * ((float)wallTextureHeight / wallHeight);

			uint32_t texelColor = wallTextureBuffer[(wallTextureWidth * textureOffsetY) + textureOffsetX];

			if(rays[x].wasHitVertical)
			{
				changeColorShading(&texelColor, 0.7);
			}

			drawPixel(x, y, texelColor);
		}

		upng_t* floorTexture = textures[8];
		int floorTextureWidth = upng_get_width(floorTexture);
		int floorTextureHeight = upng_get_height(floorTexture);
		uint32_t* floorTextureBuffer = (uint32_t*)upng_get_buffer(floorTexture);


		for (int y = wallBotY; y < WINDOW_HEIGHT; y++) {

			float diagnolDistance = fabs(y - DIST_PROJ_PLANE);
			float floorProjectionHeight = (TILE_SIZE / diagnolDistance ) * DIST_PROJ_PLANE;

			int distanceFromTop = y + (floorProjectionHeight / 2) - (WINDOW_HEIGHT / 2);
			int textureOffsetY = diagnolDistance * ((float)floorTextureWidth / floorProjectionHeight);

			uint32_t texelColor = floorTextureBuffer[((floorTextureWidth * textureOffsetY) + textureOffsetX) % (64 * 64)];

			//drawPixel(x, y, texelColor);
			drawPixel(x, y, 0xFF444444);
		}

	}
}


uint32_t createRGBA(int a, int r, int g, int b)
{   
    return ((a & 0xff) << 24) + ((r & 0xff) << 16) + ((g & 0xff) << 8)
           + (b & 0xff);
}


