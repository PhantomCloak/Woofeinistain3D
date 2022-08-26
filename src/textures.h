#ifndef TEXTURES_H
#define TEXTURES_H

#include <stdint.h>
#include "constans.h"
#include "upng.h"

typedef struct{
	int width;
	int height;
	uint32_t* texture_buffer;
	upng_t* upngTexture;
} texture_t;

texture_t wallTextures[NUM_TEXTURES];
void loadWallTextures();
void clearAllTextures();
#endif
