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
	"./images/wood1.png", 
	"./images/barrel.png",
	"./images/light.png",
	"./images/table.png",
	"./images/guard.png",
	"./images/armor.png",
};


void loadTextures()
{
	for(int i = 0; i < NUM_TEXTURES; i++)
	{
		upng_t* upng;
		upng = upng_new_from_file(textureFileNames[i]);

		if(upng == NULL)
		{
			printf("Error loading texture file %s\n", textureFileNames[i]);
			continue;
		}

		upng_decode(upng);

		if(upng_get_error(upng) != UPNG_EOK)
		{
			printf("Error loading texture %s\n", textureFileNames[i]);
			continue;
		}
		textures[i] = upng;
	}
}

void freeTextures()
{
	for(int i = 0; i < NUM_TEXTURES; i++)
	{
		upng_free(textures[i]);
	}
}

void changeColorShading(uint32_t* color, float shading)
{
    uint32_t a = (*color & 0xFF000000);
    uint32_t r = (*color & 0x00FF0000) * shading;
    uint32_t g = (*color & 0x0000FF00) * shading;
    uint32_t b = (*color & 0x000000FF) * shading;

    *color = a | (r & 0x00FF0000) | (g & 0x0000FF00) | (b & 0x000000FF);
}
