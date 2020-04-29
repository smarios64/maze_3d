#ifndef _COMMON_H_
#define _COMMON_H_

#define SCR_WIDTH 1280.0f
#define SCR_HEIGHT 720.0f

#define MAZE_WIDTH 16
#define MAZE_HEIGHT 9

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define ABS(a) ((a) >= 0 ? (a) : -(a))

#define WALL_SIZE 1.5f
#define COLUMN_SIZE 0.2f
#define HALF_COLUMN_SIZE (COLUMN_SIZE / 2.0f)

#define MINIMAP_WIDTH 0.5f
#define MINIMAP_HEIGHT 0.5f
#define MINIMAP_X    0.4f
#define MINIMAP_Y    0.9f

#endif // _COMMON_H_