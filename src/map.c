#include "map.h"

static const int map[MAP_NUM_ROWS][MAP_NUM_COLS] = {
    {1, 1, 1, 4, 1, 4, 1, 4, 1, 1, 1, 6, 5 ,5, 6, 6, 6, 6, 6, 6},
    {1, 4, 4, 4, 0, 4, 4, 4, 0, 4, 0, 6, 0, 0, 6, 0, 0, 0, 0, 6},
    {1, 4, 0, 0, 0, 0, 0, 4, 0, 4, 0, 6, 0, 0, 6, 0, 0, 0, 0, 6},
    {1, 2, 0, 0, 0, 0, 0, 4, 0, 4, 0, 6, 0, 0, 6, 0, 0, 0, 0, 6},
    {1, 2, 0, 0, 0, 0, 0, 4, 0, 4, 0, 6, 0, 0, 6, 0, 0, 0, 0, 6},
    {1, 8, 0, 0, 0, 0, 0, 16, 4, 4, 4, 6, 0, 0, 6, 0, 6, 6, 6, 6},
    {1, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 4, 0, 0, 0, 0, 0, 16, 4, 4, 4, 6, 0, 0, 6, 0, 6, 6, 6, 6},
    {1, 2, 0, 0, 0, 0, 0, 4, 0, 4, 0, 6, 0, 0, 6, 0, 0, 0, 0, 6},
    {1, 4, 0, 0, 0, 0, 0, 4, 0, 4, 0, 6, 0, 0, 6, 0, 0, 0, 0, 6},
    {1, 4, 4, 4, 0, 4, 4, 4, 0, 4, 0, 6, 0, 0, 6, 0, 0, 0, 0, 6},
    {1, 1, 1, 4, 1, 4, 1, 4, 1, 4, 1, 6, 5, 5, 6, 6, 6, 6, 6, 1}
};

bool isInsideWall(float x, float y) {
    if (x < 0 || x >= MAP_NUM_COLS * TILE_SIZE || y < 0 || y >= MAP_NUM_ROWS * TILE_SIZE) {
        return true;
    }
    int mapGridIndexX = floor(x / TILE_SIZE);
    int mapGridIndexY = floor(y / TILE_SIZE);
    return map[mapGridIndexY][mapGridIndexX] != 0;
}

bool isInsideMap(float x, float y) {
    return (x >= 0 && x <= MAP_NUM_COLS * TILE_SIZE && y >= 0 && y <= MAP_NUM_ROWS * TILE_SIZE);
}

int getMapAt(int y, int x) {
    return map[y][x];
}

void renderMap() {
    for (int i = 0; i < MAP_NUM_ROWS; i++) {
        for (int j = 0; j < MAP_NUM_COLS; j++) {
            int tileX = j * TILE_SIZE;
            int tileY = i * TILE_SIZE;
            uint32_t tileColor = map[i][j] != 0 ? 0xFF000000 : 0xFFFFFFFF;
            drawRectangle(
                tileX * MINIMAP_SCALE_FACTOR,
                tileY * MINIMAP_SCALE_FACTOR,
                TILE_SIZE * MINIMAP_SCALE_FACTOR,
                TILE_SIZE * MINIMAP_SCALE_FACTOR,
                tileColor
            );
        }
    }
}

