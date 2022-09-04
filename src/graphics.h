#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <SDL2/SDL.h>
#include <stdbool.h>

bool initializeWindow(int windowWidth, int windowHeight);
void destroyWindow();
void clearColorBuffer(uint32_t color);
void renderColorBuffer();
void drawPixel(int x, int y, uint32_t color);
void drawRectangle(int x, int y, int width, int height, uint32_t color);
void drawLine(int x0, int y0, int x1, int y1, uint32_t color);
#endif
