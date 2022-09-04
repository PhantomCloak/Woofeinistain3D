#ifndef SPRITE_H
#define SPRITE_H

#include "graphics.h"
#include "player.h"
#include "mathUtil.h"
#include "textures.h"
#include "ray.h"
typedef struct {
	float x;
	float y;
	float distance;
	float angle;
	int textureIndex;
	bool visible;
} sprite_t;

void renderSprites();
void renderMapSprites();
#endif
