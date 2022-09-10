#include "player.h"

animation_t * anim;
player_t player = {
	.x = WINDOW_WIDTH / 1.8,
	.y = WINDOW_HEIGHT / 1.8,
	.width = 5,
	.height = 5,
	.turnDirection = 0,
	.walkDirection = 0,
	.rotationAngle = PI / 2,
	.walkSpeed = 250,
	.turnSpeed = 85 * (PI / 180),
    .currentWeapon = "handgun"
};

struct Vector2 playerNextPosition(float dt)
{
	player.rotationAngle += player.turnDirection * player.turnSpeed * dt;
	player.rotationAngle = normalizeAngle(player.rotationAngle);
	float moveStep = player.walkDirection * player.walkSpeed * dt;

	float newPlayerX = player.x + cos(player.rotationAngle) * moveStep;
	float newPlayerY = player.y + sin(player.rotationAngle) * moveStep;
	
	struct Vector2 nextPos;
	nextPos.x = newPlayerX;
	nextPos.y = newPlayerY;

	return nextPos;
}

void playerSetPosition(int x, int y)
{
    player.x = x;
    player.y = y;
}

float tlf = 0;
int animCtx = 0;
upng_t* texture;
int weaponFireAimRound = 0;

void playerInitAnims()
{
    if(anim == NULL)
    {
        tlf = SDL_GetTicks();
        anim = getAnimationHandle(player.currentWeapon);
        texture = anim->clips[0].texture;
    }
}

int lastStateShoot = 0;
void playerTick()
{
    int timeToWait = (SDL_GetTicks() - tlf);

    if(timeToWait < 150)
    {
        return;
    }
    
    anim = getAnimationHandle(player.currentWeapon);
    // 0 = Idle
    // 1 = Transistion to Fire
    // 2 = Fire
    // 3 = Recoil Start
    // 4 = Recoil End
    if(player.isShooting)
    {
        if(lastStateShoot < 2)
        {
            lastStateShoot++;
            texture = anim->clips[lastStateShoot].texture;
        }
        else
        {
            int fireIndex = 2 + (weaponFireAimRound % 2);
            texture = anim->clips[fireIndex].texture;

            lastStateShoot = fireIndex;
            weaponFireAimRound++;
        }
    }
    else
    {
        if(lastStateShoot == 0 || lastStateShoot == 5)
        {
            texture = anim->clips[lastStateShoot].texture;
            lastStateShoot = 0;
        }
        else if(lastStateShoot < 4)
        {
            lastStateShoot++;
            texture = anim->clips[lastStateShoot].texture;
        }
        else
        {
            lastStateShoot = (lastStateShoot + 1) % 5; // should be 0
        }
    }

    tlf = SDL_GetTicks();
}

void renderPlayer()
{
    drawRectangle(player.x * MINIMAP_SCALE_FACTOR, player.y * MINIMAP_SCALE_FACTOR,
            player.width,
            player.height,0xFFFF0000);
}

void renderPlayerGun()
{
	int yEnd = (WINDOW_HEIGHT + 64) * 10;

	int weaponX = (WINDOW_WIDTH / 2);

	int gunTexWidth = 512;
	int gunTexHeight = 512;

	int gunX = ((WINDOW_WIDTH / 2) - (gunTexWidth / 2));
	int gunY = (WINDOW_HEIGHT) - gunTexHeight;

	drawTexture(gunX , gunY, gunTexWidth, gunTexHeight, texture);
}
