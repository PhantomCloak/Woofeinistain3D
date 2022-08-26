#include "textures.h"
#include <stdio.h>

static const char* textureFileNames[NUM_TEXTURES] = {
	"./images/redbrick.png",
	"./images/nazistone.png",
	"./images/woodhitler.png",
	"./images/graystone.png",
	"./images/colorstone.png",
	"./images/bluestone.png",
	"./images/wood.png",
	"./images/eagle.png",
};


void loadWallTextures()
{
	for(int i = 0; i < NUM_TEXTURES; i++)
	{
		upng_t* upng;
		upng = upng_new_from_file(textureFileNames[i]);

		if(upng == NULL)
			continue;

		upng_decode(upng);

		if(upng_get_error(upng) != UPNG_EOK)
			return;

		wallTextures[i].upngTexture = upng;
		wallTextures[i].width = upng_get_width(upng);
		wallTextures[i].height = upng_get_height(upng);
		wallTextures[i].texture_buffer = (uint32_t*)upng_get_buffer(upng);
	}
}

void clearAllTextures()
{
	for(int i = 0; i < NUM_TEXTURES; i++)
	{
		upng_free(wallTextures[i].upngTexture);
	}
}
