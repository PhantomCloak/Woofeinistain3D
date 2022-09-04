#include "sprite.h"

#define NUM_SPRITES 3

static sprite_t sprites[NUM_SPRITES] = {
	{.x = 200, .y = 600, .textureIndex = 9}, // barell
	{.x = 200, .y = 500, .textureIndex = 11}, // barell
	{.x = 300, .y = 500, .textureIndex = 12} // barell
};

void renderSprites()
{
	for(int i = 0; i < NUM_SPRITES; i++)
	{
		sprite_t sprite = sprites[i];
		drawRectangle(sprite.x * MINIMAP_SCALE_FACTOR, sprite.y * MINIMAP_SCALE_FACTOR, 7, 7,(sprite.visible) ? 0xFFFF00FF : 0xFF444444);
	}
}

void renderMapSprites()
{
	sprite_t visibleSprites[NUM_SPRITES];
	int numVisibleSprites = 0;
	for(int i = 0; i < NUM_SPRITES; i++)
	{
		float angleSprite = player.rotationAngle - atan2(sprites[i].y - player.y, sprites[i].x - player.x);

		if(angleSprite > PI)
			angleSprite -= TWO_PI;
		if(angleSprite < -PI)
			angleSprite += TWO_PI;
		angleSprite = fabs(angleSprite);

		if(angleSprite < (FOV_ANGLE / 2) + 0.2)
		{
			sprites[i].angle = angleSprite;
			sprites[i].distance = distanceBetweenPoints(sprites[i].x, sprites[i].y, player.x, player.y);
			sprites[i].visible = true;

			visibleSprites[numVisibleSprites] = sprites[i];
			numVisibleSprites++;
		}
		else
		{
			sprites[i].visible = false;
		}
	}

	for (int i = 0; i < numVisibleSprites - 1; i++)
		for (int j = i + 1; j < numVisibleSprites; j++)
			if (visibleSprites[i].distance < visibleSprites[j].distance)
			{
				sprite_t temp = visibleSprites[i];
				visibleSprites[i] = visibleSprites[j];
				visibleSprites[j] = temp;
			}


	for(int i = 0; i < numVisibleSprites; i++)
	{
		sprite_t sprite = visibleSprites[i];
		float spriteHeight = (TILE_SIZE / sprite.distance) * DIST_PROJ_PLANE;
		float spriteWidth = spriteHeight;

		float spriteTopY = (WINDOW_HEIGHT / 2) - (spriteHeight / 2);
		spriteTopY = spriteTopY < 0 ? 0 : spriteTopY;

		float spriteBotY = (WINDOW_HEIGHT / 2) + (spriteHeight / 2);
		spriteBotY = spriteBotY > WINDOW_HEIGHT ? WINDOW_HEIGHT : spriteBotY;

		float spriteAngle = atan2(sprite.y - player.y, sprite.x - player.x) - player.rotationAngle;
		float spritePosX = tan(spriteAngle) * DIST_PROJ_PLANE;

		float spriteLeftX = (WINDOW_WIDTH / 2) + spritePosX -(spriteWidth / 2);
		float spriteRightX = spriteLeftX + spriteWidth;

		int textureWidth = upng_get_width(textures[sprite.textureIndex]);
		int textureHeight = upng_get_height(textures[sprite.textureIndex]);

		uint32_t* spriteTextureBuffer = (uint32_t*)upng_get_buffer(textures[sprite.textureIndex]);
		for(int x = spriteLeftX; x < spriteRightX;x++)
		{
			float texelWidth = (textureWidth / spriteWidth);
			int textureOffsetX = (x - spriteLeftX) * texelWidth;

			for(int y = spriteTopY; y < spriteBotY;y++)
			{
				if(x > 0 && x < WINDOW_WIDTH && y > 0 && y < WINDOW_HEIGHT)
				{
					int distanceFromTop = y + (spriteHeight / 2) - (WINDOW_HEIGHT / 2);
					int textureOffsetY = distanceFromTop * (textureHeight / spriteHeight);
					uint32_t texelColor = spriteTextureBuffer[(textureWidth * textureOffsetY) + textureOffsetX];

					if(sprite.distance < rays[x].distance && texelColor != 0xFFFF00FF)
						drawPixel(x,y,texelColor);
				}
			}
		}
	}
}
