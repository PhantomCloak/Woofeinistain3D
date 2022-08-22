#include "constans.h"
#include <SDL.h>

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
int isGameRunning = 0;

int initializeWindow()
{
	if(SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		fprintf(stderr,"Error on initializing SDL\n");
		return 0;
	}

	window = SDL_CreateWindow("Duum3D",
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			WINDOW_HEIGHT,
			WINDOW_WIDTH,
			SDL_WINDOW_BORDERLESS);

	if(!window)
	{
		fprintf(stderr, "Error creating window");
		return 0;
	}

	renderer = SDL_CreateRenderer(window, -1, 0);

	if(!renderer)
	{
		fprintf(stderr, "Error creating renderer");
		return 0;
	}

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

	return 1;
}

void setup()
{

}

void processInput()
{

}

void update()
{

}

void render()
{

}

int main(int argc, char* argv[])
{
	isGameRunning = initializeWindow();
	setup();

	while(isGameRunning)
	{
		processInput();
		update();
		render();
	}

	return 0;
}

void destroyWindow()
{
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}
