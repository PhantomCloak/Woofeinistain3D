#include <SDL2/SDL.h>
#include "SDL2/SDL_log.h"
#include "constans.h"
#include "textures.h"

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
int isGameRunning = 0;

float tickLastFrame = 0;

const int map[MAP_NUM_ROWS][MAP_NUM_COLS] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 ,1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

struct Player{
	float x;
	float y;
	float width;
	float height;
	int turnDirection;
	int walkDirection;
	float rotationAngle;
	float walkSpeed;
	float turnSpeed;
} player;

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
Uint32* texture = NULL;
Uint32 textures[NUM_TEXTURES]
SDL_Texture* colorBufferTexture;

int isInsideWall(int x, int y)
{
	int row = floor(y / TILE_SIZE);
	int column = floor(x / TILE_SIZE);

	return map[row][column] != 0;
}

float normalizeAngle(float rayAngle)
{
	rayAngle = remainder(rayAngle, TWO_PI);
	if(rayAngle < 0)
	{
		rayAngle = 	TWO_PI + rayAngle;
	}

	return rayAngle;
}

float distanceBetweenPoints(float x1, float y1, float x2, float y2)
{
	return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

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

	while(nextHorizontalTouchX >= 0 && nextHorizontalTouchX <= WINDOW_WIDTH && nextHorizontalTouchY >= 0 && nextHorizontalTouchY <= WINDOW_HEIGHT)
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

	while(nextVerticalTouchX >= 0 && nextVerticalTouchX <= WINDOW_WIDTH && nextVerticalTouchY >= 0 && nextVerticalTouchY <= WINDOW_HEIGHT)
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
	float rayAngle = player.rotationAngle - (FOV_ANGLE / 2);

	for(int stripId = 0; stripId < NUM_RAYS; stripId++)
	{
		castRay(rayAngle, stripId);
		rayAngle += FOV_ANGLE / NUM_RAYS;
	}
}

void movePlayer(float dt)
{
	player.rotationAngle += player.turnDirection * player.turnSpeed * dt;
   	float moveStep = player.walkDirection * player.walkSpeed * dt;

    float newPlayerX = player.x + cos(player.rotationAngle) * moveStep;
    float newPlayerY = player.y + sin(player.rotationAngle) * moveStep;

	int res = isInsideWall(newPlayerX, newPlayerY);
	if(res)
	{
		return;
	}

    player.x = newPlayerX;
    player.y = newPlayerY;
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
	player.x = WINDOW_WIDTH / 2;
	player.y = WINDOW_HEIGHT / 2;
	player.height = 5;
	player.width = 5;
	player.turnDirection = 0;
	player.walkDirection = 0;
	player.turnSpeed = 90 * (PI / 180);
	player.walkSpeed = 250;
	player.rotationAngle = PI / 2;

	colorBuffer = (Uint32*)malloc(sizeof(Uint32) * (Uint32)WINDOW_WIDTH * (Uint32)WINDOW_HEIGHT);
	texture = (Uint32*)malloc(sizeof(Uint32) * (Uint32)TEXTURE_HEIGHT * (Uint32)TEXTURE_WIDTH);

	colorBufferTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,SDL_TEXTUREACCESS_STREAMING , WINDOW_WIDTH, WINDOW_HEIGHT);

	for(int x = 0; x < TEXTURE_WIDTH; x++)
		for(int y = 0; y < TEXTURE_HEIGHT; y++)
		{
			texture[(TEXTURE_WIDTH * y) + x] = (x % 8 && y % 8) ? 0xFFFF0000 : 0xFF000000; 
		}
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
	while(!SDL_TICKS_PASSED(SDL_GetTicks(), tickLastFrame + FRAME_TIME_LENGTH));

	int timeToWait = FRAME_TIME_LENGTH - (SDL_GetTicks() - tickLastFrame);

	if(timeToWait > 0 && timeToWait <= FRAME_TIME_LENGTH)
	{
		SDL_Delay(timeToWait);
	}

	float dt = (SDL_GetTicks() - tickLastFrame) / 1000.0f;
	tickLastFrame = SDL_GetTicks();

	movePlayer(dt);
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
		float dist = (WINDOW_WIDTH / 2) / tan(FOV_ANGLE / 2);
		float correctWallDistance = rays[i].distance * cos(rays[i].rayAngle - player.rotationAngle);
		float projectedHeight = (TILE_SIZE / correctWallDistance) * dist;

		int wallTopPixel = (WINDOW_HEIGHT / 2)  - (projectedHeight / 2);
		wallTopPixel = wallTopPixel < 0 ? 0 : wallTopPixel;

		int wallBotPixel = (WINDOW_HEIGHT / 2)  + (projectedHeight / 2);
		wallBotPixel = wallBotPixel < 0 ? 0 : wallBotPixel;

		int textureOffsetX;
		if(rays[i].wasHitVertical)
		{
			textureOffsetX = (int)rays[i].wallHitY % TILE_SIZE;
		}
		else
		{
			textureOffsetX = (int)rays[i].wallHitX % TILE_SIZE;
		}

		for(int y = wallTopPixel; y < wallBotPixel; y++)
		{
			int distanceFromTop = y + (projectedHeight / 2) - (WINDOW_HEIGHT / 2);
			int textureOffsetY = distanceFromTop * ((float)TEXTURE_HEIGHT / projectedHeight);

			Uint32 texelColor = texture[(TEXTURE_WIDTH * textureOffsetY) + textureOffsetX];
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
	SDL_DestroyTexture(colorBufferTexture);

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}
