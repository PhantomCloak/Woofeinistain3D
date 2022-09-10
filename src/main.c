#include <SDL2/SDL.h>
#include "SDL2/SDL_log.h"
#include <stdbool.h>
#include "constans.h"
#include "graphics.h"
#include "map.h"
#include "player.h"
#include "ray.h"
#include "sprite.h"
#include "wall.h"
#include "textures.h"
#include "animator.h"

bool isGameRunning = false;
float tickLastFrame = 0;

void processInput()
{
	SDL_Event event;
	SDL_PollEvent(&event);

	switch(event.type)
	{
		case SDL_QUIT:
			{
				isGameRunning = 0;
			}break;
		case SDL_KEYDOWN:
			{
				if(event.key.keysym.sym == SDLK_ESCAPE)
					isGameRunning = false;
				if(event.key.keysym.sym == SDLK_UP)
					player.walkDirection = 1;
				if(event.key.keysym.sym == SDLK_DOWN)
					player.walkDirection = -1;
				if(event.key.keysym.sym == SDLK_RIGHT)
					player.turnDirection = 1;
				if(event.key.keysym.sym == SDLK_LEFT)
					player.turnDirection = -1;
				if(event.key.keysym.sym == SDLK_SPACE)
					player.isShooting = true;
				if(event.key.keysym.sym == SDLK_1)
                    player.currentWeapon = "handgun";
				if(event.key.keysym.sym == SDLK_2)
                    player.currentWeapon = "smg";
			}break;
		case SDL_KEYUP:
			{
				if(event.key.keysym.sym == SDLK_UP)
					player.walkDirection = 0;
				if(event.key.keysym.sym == SDLK_DOWN)
					player.walkDirection = 0;
				if(event.key.keysym.sym == SDLK_RIGHT)
					player.turnDirection = 0;
				if(event.key.keysym.sym == SDLK_LEFT)
					player.turnDirection = 0;
				if(event.key.keysym.sym == SDLK_SPACE)
					player.isShooting = false;
			}break;
	}
}

void update()
{
	int timeToWait = FRAME_TIME_LENGTH - (SDL_GetTicks() - tickLastFrame);

	if(timeToWait > 0 && timeToWait <= 500)
	{
		SDL_Delay(timeToWait);
	}

	float dt = (SDL_GetTicks() - tickLastFrame) / 1000.0f;
	tickLastFrame = SDL_GetTicks();

	// check shooting
	playerTick();
	struct Vector2 pPos = playerNextPosition(dt);

	if(!isInsideWall(pPos.x, pPos.y))
	{
		playerSetPosition(pPos.x, pPos.y);
	}

	castAllRays();
}

void render()
{
	clearColorBuffer(0xFF000000);

	renderWalls();
	//render sprites
	
	renderMap();
	renderRays();
	renderMapSprites();
	renderSprites();
	renderPlayer();
	renderPlayerGun();

	renderColorBuffer();
}

void releaseResources()
{
	freeTextures();
	destroyWindow();
}

void testsound()
{
	SDL_AudioSpec wavSpec;
	Uint32 wavLength;
	Uint8 *wavBuffer;
	//SDL_LoadWAV("./sounds/wandering.wav", &wavSpec, &wavBuffer, &wavLength);
	SDL_AudioDeviceID deviceId = SDL_OpenAudioDevice(NULL, 0, &wavSpec, NULL, 0);
	int success = SDL_QueueAudio(deviceId, wavBuffer, wavLength);
	SDL_PauseAudioDevice(deviceId, 0);

}

int main(int argc, char* argv[])
{
	isGameRunning = initializeWindow(WINDOW_WIDTH, WINDOW_HEIGHT);

	loadTextures();
	loadAnimations();
    playerInitAnims();
	testsound();
	while(isGameRunning)
	{
		processInput();
		update();
		render();
	}

	releaseResources();
	return 0; }

