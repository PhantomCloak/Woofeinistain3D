#ifndef TEXTURES_H
#define TEXTURES_H

#include <stdint.h>
#include "upng.h"

#define NUM_TEXTURES 16

upng_t* textures[NUM_TEXTURES];

void loadTextures();
void freeTextures();
void freeTextures();

void changeColorShading(uint32_t* color, float shading);
#endif
