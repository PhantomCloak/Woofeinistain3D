#ifndef MAP_H
#define MAP_H
#include <stdbool.h>
#include <math.h>
#include "constans.h"

extern const int map[MAP_NUM_ROWS][MAP_NUM_COLS];
extern bool isInsideWall(int x, int y);
#endif
