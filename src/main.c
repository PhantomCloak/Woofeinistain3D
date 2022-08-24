#include <SDL2/SDL.h>
#include "SDL2/SDL_log.h"
#include "constans.h"

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

int isInsideWall(int x, int y)
{
	int row = floor(y / TILE_SIZE);
	int column = floor(x / TILE_SIZE);

	return map[row][column] == 1;
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

float castRay(float rayAngle, int index)
{
	rayAngle = normalizeAngle(rayAngle);

	int isRayFacingDown = rayAngle > 0 && rayAngle < PI;
	int isRayFacingUp = !isRayFacingDown;
	
	int isRayFacingRight = rayAngle < 0.5 * PI || rayAngle > 1.5 * PI;
	int isRayFacingLeft = !isRayFacingDown;

	float xIntercept;
	float yIntercept;
	float xStep;
	float yStep;

	int horizontalWallHitX;
	int horizontalWallHitY;
	int horizontalWallContent;

	int foundHorizontalWallHit;
	int horizontalWallContent;

	yIntercept = floor(player.y / TILE_SIZE) * TILE_SIZE;
	yIntercept += isRayFacingDown ? TILE_SIZE : 0; 

	xIntercept = player.x + (yIntercept - player.y) / tan(rayAngle);

	yStep = TILE_SIZE;
	xStep *= isRayFacingUp ? -1 : 1;

	xStep = TILE_SIZE / tan(rayAngle);
	xStep *= (isRayFacingLeft && xStep > 0) ? -1 : 1;
	xStep *= (isRayFacingRight && xStep < 0) ? -1 : 1;

	float nextHorizontalTouchX = xIntercept;
	float nextHorizontalTouchY = yIntercept;

	while(nextHorizontalTouchX >= 0 && nextHorizontalTouchX <= WINDOW_WIDTH && nextHorizontalTouchY >= 0 && nextHorizontalTouchY < WINDOW_HEIGHT)
	{
		float xToCheck = nextHorizontalTouchX;
		float yToCheck = nextHorizontalTouchY + (isRayFacingUp) ? -1 : 0;

		if(mapHasWallAt(xToCheck, yToCheck))
		{
			horizontalWallHitX = nextHorizontalTouchX;
			horizontalWallHitY = nextHorizontalTouchY;
			foundHorizontalWallHit = 1;
			horizontalWallContent = map[(int)floor(yToCheck / TILE_SIZE)][(int)floor(xToCheck / TILE_SIZE)];
		}
		else
		{
			nextHorizontalTouchX += xStep;
			nextHorizontalTouchY += yStep;
		}
	}
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
	printf("%d", res);
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
	player.turnSpeed = 45 * (PI / 180);
	player.walkSpeed = 100;
	player.rotationAngle = PI / 2;
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

void render()
{
	SDL_SetRenderDrawColor(renderer, 0,0,0,255);
	SDL_RenderClear(renderer);

	renderMap();
	renderPlayer();

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
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}
