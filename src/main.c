#include <SDL2/SDL.h>
#include "SDL2/SDL_log.h"
#include "constans.h"
#include "helper.h"
#include "textures.h"
#include "map.h"
#include "player.h"

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
int isGameRunning = 0;
float tickLastFrame = 0;


struct Ray
{
	float rayAngle;
	float wallHitX;
	float wallHitY;
	float distance;
	int wasHitVertical;
	int rayFacingUp;
	int rayFacingDown;
	int rayFacingRight;
	int rayFacingLeft;
	int wallHitContent;
} rays[NUM_RAYS];

Uint32* colorBuffer = NULL;
SDL_Texture* colorBufferTexture;

float castRay(float rayAngle, int index)
{
	rayAngle = normalizeAngle(rayAngle);

	int isRayFacingDown = rayAngle > 0 && rayAngle < PI;
	int isRayFacingUp = !isRayFacingDown;
	
	int isRayFacingRight = rayAngle < 0.5 * PI || rayAngle > 1.5 * PI;
	int isRayFacingLeft = !isRayFacingRight;

	float xIntercept;
	float yIntercept;
	float xStep;
	float yStep;

	int foundHorizontalWallHit = 0;
	int horizontalWallHitX = 0;
	int horizontalWallHitY = 0;
	int horizontalWallContent = 0;


	yIntercept = floor(player.y / TILE_SIZE) * TILE_SIZE;
	yIntercept += isRayFacingDown ? TILE_SIZE : 0; 

	xIntercept = player.x + (yIntercept - player.y) / tan(rayAngle);

	yStep = TILE_SIZE;
	yStep *= isRayFacingUp ? -1 : 1;

	xStep = TILE_SIZE / tan(rayAngle);
	xStep *= (isRayFacingLeft && xStep > 0) ? -1 : 1;
	xStep *= (isRayFacingRight && xStep < 0) ? -1 : 1;

	float nextHorizontalTouchX = xIntercept;
	float nextHorizontalTouchY = yIntercept;

	while(nextHorizontalTouchX >= 0 && nextHorizontalTouchX <= (MAP_NUM_COLS * TILE_SIZE) && nextHorizontalTouchY >= 0 && nextHorizontalTouchY <= (MAP_NUM_ROWS * TILE_SIZE))
	{
		float xToCheck = nextHorizontalTouchX;
		float yToCheck = nextHorizontalTouchY + (isRayFacingUp ? -1 : 0);

		if(isInsideWall(xToCheck, yToCheck))
		{
			horizontalWallHitX = nextHorizontalTouchX;
			horizontalWallHitY = nextHorizontalTouchY;
			horizontalWallContent = map[(int)floor(yToCheck / TILE_SIZE)][(int)floor(xToCheck / TILE_SIZE)];
			foundHorizontalWallHit = 1;
			break;
		}
		else
		{
			nextHorizontalTouchX += xStep;
			nextHorizontalTouchY += yStep;
		}
	}

	// Vertical check
	//--------------------------------------------------------------------------
	int foundVerticalWallHit = 0;
	int verticalWallHitX = 0;
	int verticalWallHitY = 0;
	int verticalWallContent = 0;

	xIntercept = floor(player.x / TILE_SIZE) * TILE_SIZE;
	xIntercept += isRayFacingRight ? TILE_SIZE : 0; 

	yIntercept = player.y + (xIntercept - player.x) * tan(rayAngle);

	xStep = TILE_SIZE;
	xStep *= isRayFacingLeft ? -1 : 1;

	yStep = TILE_SIZE * tan(rayAngle);
	yStep *= (isRayFacingUp && yStep > 0) ? -1 : 1;
	yStep *= (isRayFacingDown && yStep < 0) ? -1 : 1;

	float nextVerticalTouchX = xIntercept;
	float nextVerticalTouchY = yIntercept;

	while(nextVerticalTouchX >= 0 && nextVerticalTouchX <= (MAP_NUM_COLS * TILE_SIZE) && nextVerticalTouchY >= 0 && nextVerticalTouchY <= (MAP_NUM_ROWS * TILE_SIZE))
	{
		float xToCheck = nextVerticalTouchX + (isRayFacingLeft ? -1 : 0);
		float yToCheck = nextVerticalTouchY;

		if(isInsideWall(xToCheck, yToCheck))
		{
			verticalWallHitX = nextVerticalTouchX;
			verticalWallHitY = nextVerticalTouchY;
			verticalWallContent = map[(int)floor(yToCheck / TILE_SIZE)][(int)floor(xToCheck / TILE_SIZE)];
			foundVerticalWallHit = 1;
			break;
		}
		else
		{
			nextVerticalTouchX += xStep;
			nextVerticalTouchY += yStep;
		}
	}

	// calc smallest one
	float horizontalDistance = foundHorizontalWallHit ? distanceBetweenPoints(player.x, player.y, horizontalWallHitX, horizontalWallHitY) : FLT_MAX;
	float verticalDistance = foundVerticalWallHit ? distanceBetweenPoints(player.x, player.y, verticalWallHitX, verticalWallHitY) : FLT_MAX;

	if(verticalDistance < horizontalDistance)
	{
		rays[index].distance = verticalDistance;
		rays[index].wallHitX = verticalWallHitX;
		rays[index].wallHitY = verticalWallHitY;
		rays[index].wallHitContent = verticalWallContent;
		rays[index].wasHitVertical = 1;
	}
	else
	{
		rays[index].distance = horizontalDistance;
		rays[index].wallHitX = horizontalWallHitX;
		rays[index].wallHitY = horizontalWallHitY;
		rays[index].wallHitContent = horizontalWallContent;
		rays[index].wasHitVertical = 0;
	}

	rays[index].rayAngle = rayAngle;
	rays[index].rayFacingDown = isRayFacingDown;
	rays[index].rayFacingUp = isRayFacingUp;
	rays[index].rayFacingLeft = isRayFacingLeft;
	rays[index].rayFacingRight = isRayFacingRight;
}

void castAlLRays()
{
	for(int col = 0; col < NUM_RAYS; col++)
	{
		float rayAngle = player.rotationAngle + atan((col - NUM_RAYS / 2) / DIST_PROJ_PLANE);
		castRay(rayAngle, col);
	}
}

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
			WINDOW_WIDTH,
			WINDOW_HEIGHT,
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

void renderRays()
{
	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	for(int i = 0; i < NUM_RAYS; i++)
	{
		SDL_RenderDrawLine(renderer, MINIMAP_SCALE_FACTOR * player.x, MINIMAP_SCALE_FACTOR * player.y, MINIMAP_SCALE_FACTOR * rays[i].wallHitX, MINIMAP_SCALE_FACTOR * rays[i].wallHitY);
	}
}

void renderPlayer()
{
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_Rect playerRect = {
		player.x * MINIMAP_SCALE_FACTOR,
		player.y * MINIMAP_SCALE_FACTOR,
		player.width * MINIMAP_SCALE_FACTOR,
		player.height * MINIMAP_SCALE_FACTOR
	};

	SDL_RenderFillRect(renderer, &playerRect);

	SDL_RenderDrawLine(renderer,
			MINIMAP_SCALE_FACTOR * player.x,
			MINIMAP_SCALE_FACTOR * player.y,
			MINIMAP_SCALE_FACTOR * player.x + cos(player.rotationAngle) * 40,
			MINIMAP_SCALE_FACTOR * player.y + sin(player.rotationAngle)  * 40);
}

void renderMap()
{
	for(int row = 0; row < MAP_NUM_ROWS; row++)
		for(int column = 0; column < MAP_NUM_COLS; column++)
		{
			int tileX = column * TILE_SIZE;
			int tileY = row * TILE_SIZE;
			int tileColor = map[row][column] != 0 ? 255 : 0;

			SDL_SetRenderDrawColor(renderer, tileColor, tileColor, tileColor, 255);
			SDL_Rect mapTileRect = {
				tileX * MINIMAP_SCALE_FACTOR,
				tileY * MINIMAP_SCALE_FACTOR,
				TILE_SIZE * MINIMAP_SCALE_FACTOR,
				TILE_SIZE * MINIMAP_SCALE_FACTOR
			};
			SDL_RenderFillRect(renderer, &mapTileRect);
		}
}


void setup()
{
	player.x = (MAP_NUM_COLS / 2) * TILE_SIZE;
	player.y = (MAP_NUM_ROWS / 2) * TILE_SIZE;
	player.height = 5;
	player.width = 5;
	player.turnDirection = 0;
	player.walkDirection = 0;
	player.turnSpeed = 90 * (PI / 180);
	player.walkSpeed = 250;
	player.rotationAngle = PI / 2;

	colorBuffer = (Uint32*)malloc(sizeof(Uint32) * (Uint32)WINDOW_WIDTH * (Uint32)WINDOW_HEIGHT);
	loadWallTextures();

	colorBufferTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32,SDL_TEXTUREACCESS_STREAMING , WINDOW_WIDTH, WINDOW_HEIGHT);
}

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
					isGameRunning = 0;
				if(event.key.keysym.sym == SDLK_UP)
					player.walkDirection = +1;
				if(event.key.keysym.sym == SDLK_DOWN)
					player.walkDirection = -1;
				if(event.key.keysym.sym == SDLK_RIGHT)
					player.turnDirection = +1;
				if(event.key.keysym.sym == SDLK_LEFT)
					player.turnDirection = -1;
			}break;
		case SDL_KEYUP:
			{
				if(event.key.keysym.sym == SDLK_ESCAPE)
					isGameRunning = 0;
				if(event.key.keysym.sym == SDLK_UP)
					player.walkDirection = 0;
				if(event.key.keysym.sym == SDLK_DOWN)
					player.walkDirection = 0;
				if(event.key.keysym.sym == SDLK_RIGHT)
					player.turnDirection = 0;
				if(event.key.keysym.sym == SDLK_LEFT)
					player.turnDirection = 0;
			}break;
	}
}

void update()
{
	int timeToWait = FRAME_TIME_LENGTH - (SDL_GetTicks() - tickLastFrame);

	if(timeToWait > 0 && timeToWait <= FRAME_TIME_LENGTH)
	{
		SDL_Delay(timeToWait);
	}

	float dt = (SDL_GetTicks() - tickLastFrame) / 1000.0f;
	tickLastFrame = SDL_GetTicks();

	struct Vector2 pPos = playerNextPosition(dt);

	if(!isInsideWall(pPos.x, pPos.y))
	{
		playerSetPosition(pPos.x, pPos.y);
	}

	castAlLRays();
}

void clearColorBuffer(Uint32 color)
{
	for(int i = 0; i < WINDOW_WIDTH; i++)
		for(int j = 0; j < WINDOW_HEIGHT; j++)
		{
			int index = (WINDOW_WIDTH * j) + i;
			if(j < WINDOW_HEIGHT / 2)
			{
				colorBuffer[index]  = 0xFF3b3b3b;
			}
			else
			{
				colorBuffer[index]  = 0xFF737373;
			}
		}
}

void renderColorBuffer()
{
	SDL_UpdateTexture(colorBufferTexture, NULL, colorBuffer, (int)(WINDOW_WIDTH * sizeof(Uint32)));
	SDL_RenderCopy(renderer, colorBufferTexture, NULL, NULL);
}

void renderWalls()
{
	for(int i = 0; i < NUM_RAYS; i++)
	{
		float correctWallDistance = rays[i].distance * cos(rays[i].rayAngle - player.rotationAngle);
		float projectedHeight = (TILE_SIZE / correctWallDistance) * DIST_PROJ_PLANE;

		int wallHeight = (int)projectedHeight;

		int wallTopPixel = (WINDOW_HEIGHT / 2)  - (wallHeight / 2);
		wallTopPixel = wallTopPixel < 0 ? 0 : wallTopPixel;

		int wallBotPixel = (WINDOW_HEIGHT / 2)  + (wallHeight / 2);
		wallBotPixel = wallBotPixel > WINDOW_HEIGHT ? WINDOW_HEIGHT : wallBotPixel;

		int textureOffsetX;
		if(rays[i].wasHitVertical)
		{
			textureOffsetX = (int)rays[i].wallHitY % TILE_SIZE;
		}
		else
		{
			textureOffsetX = (int)rays[i].wallHitX % TILE_SIZE;
		}

		int texNum = rays[i].wallHitContent - 1;
		int textureWidth = wallTextures[texNum].width;
		int textureHeight = wallTextures[texNum].height;

		for(int y = wallTopPixel; y < wallBotPixel; y++)
		{
			int distanceFromTop = y + ((int)projectedHeight / 2) - (WINDOW_HEIGHT / 2);
			int textureOffsetY = distanceFromTop * ((float)textureHeight / (int)projectedHeight);
			Uint32 texelColor = wallTextures[texNum].texture_buffer[(textureWidth * textureOffsetY) + textureOffsetX];
			colorBuffer[(WINDOW_WIDTH * y)  + i] = texelColor;
		}
	}
}

void render()
{
	SDL_SetRenderDrawColor(renderer, 0,0,0,255);
	SDL_RenderClear(renderer);

	renderWalls();
	renderColorBuffer();
	clearColorBuffer(0xFF000000);

	renderMap();
	renderPlayer();
	renderRays();

	SDL_RenderPresent(renderer);
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
	free(colorBuffer);
	clearAllTextures();

	SDL_DestroyTexture(colorBufferTexture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}
