#ifndef MEMORY_H
#define MEMORY_H

#include <stdbool.h>
#include <stdint.h>

#define DIGIT_SPRITE_OFFSET (0x000)
#define BYTES_PER_DIGIT     (5)

#define SCREEN_WIDTH  (63)
#define SCREEN_HEIGHT (31)

extern uint8_t VRAM[SCREEN_HEIGHT][SCREEN_WIDTH];

uint8_t read_byte(uint16_t addr);

void write_byte(uint16_t addr, uint8_t data);

void push_word(uint16_t data);

uint16_t pop_word();

bool load_rom(const char * filename);

void init_ram();

#endif // MEMORY_H