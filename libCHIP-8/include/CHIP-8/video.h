#ifndef VIDEO_H
#define VIDEO_H

#include <stdint.h>

#define SCREEN_WIDTH  (63)
#define SCREEN_HEIGHT (31)

extern uint8_t VRAM[SCREEN_HEIGHT][SCREEN_WIDTH];

#endif // VIDEO_H