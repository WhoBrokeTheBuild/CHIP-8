#ifndef MEMORY_H
#define MEMORY_H

#include <stdbool.h>
#include <stdint.h>

extern uint8_t RAM[0xFFF];

extern uint16_t Stack[16];

uint8_t read_byte(uint16_t addr);

uint16_t read_word(uint16_t addr);

void write_byte(uint16_t addr, uint8_t data);

void write_word(uint16_t addr, uint8_t data);

void push_word(uint16_t data);

uint16_t pop_word();

bool load_rom(const char * filename);

#endif // MEMORY_H