#include "graphics.h"

static SDL_Window*  window = NULL;
static SDL_Renderer* renderer = NULL;

static Uint32* colorBuffer = NULL;
static SDL_Texture* colorBufferTexture;

static int graphicsWindowWidth;
static int graphicsWindowHeight;

bool initializeWindow(int windowWidth, int windowHeight)
{
	if(SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		fprintf(stderr,"Error on initializing SDL\n");
		return false;
	}

	window = SDL_CreateWindow("Duum3D",
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			windowWidth,
			windowHeight,
			SDL_WINDOW_RESIZABLE);

	if(!window)
	{
		fprintf(stderr, "Error creating window");
		return false;
	}

	renderer = SDL_CreateRenderer(window, -1, 0);

	if(!renderer)
	{
		fprintf(stderr, "Error creating renderer");
		return false;
	}

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

	colorBuffer = (uint32_t*)malloc(sizeof(Uint32) * windowWidth * windowHeight);
	colorBufferTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32,SDL_TEXTUREACCESS_STREAMING , windowWidth, windowHeight);

	graphicsWindowWidth = windowWidth;
	graphicsWindowHeight = windowHeight;
	return true;
}

void drawPixel(int x, int y, uint32_t color)
{
	colorBuffer[(graphicsWindowWidth * y) + x] = color;
}

void drawLine(int x0, int y0, int x1, int y1, uint32_t color)
{
	int dX = (x1 - x0);
	int dY = (y1 - y0);

	int sideLen = abs(dX) >= abs(dY) ? abs(dX) : abs(dY);

	float xInc = dX / (float)sideLen;
	float yInc = dY / (float)sideLen;

	float currentX = x0;
	float currenyY = y0;

	for(int i = 0; i < sideLen; i++)
	{
		drawPixel(round(currentX), round(currenyY), color);
		currentX += xInc;
		currenyY += yInc;
	}
}

void drawRectangle(int x, int y, int width, int height, uint32_t color)
{
	for(int i = x; i <= (x + width); i++)
		for(int j = y; j <= (y + height); j++)
		{
			drawPixel(i, j, color);
		}
}

void clearColorBuffer(uint32_t color)
{
	for (int i = 0; i < graphicsWindowWidth * graphicsWindowHeight; i++)
		colorBuffer[i] = color;
}

void renderColorBuffer()
{
	SDL_UpdateTexture(colorBufferTexture, NULL, colorBuffer, (int)(graphicsWindowWidth * sizeof(uint32_t)));
	SDL_RenderCopy(renderer, colorBufferTexture, NULL, NULL);
	SDL_RenderPresent(renderer);
}

void destroyWindow()
{
	free(colorBuffer);

	SDL_DestroyTexture(colorBufferTexture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}
