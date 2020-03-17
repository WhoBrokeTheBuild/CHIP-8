#include "memory.h"
#include "cpu.h"

#include <stdio.h>
#include <string.h>

uint8_t RAM[0xFFF] = { 0 };

uint16_t Stack[16] = { 0 };

uint8_t read_byte(uint16_t addr)
{
    return RAM[addr];
}

uint16_t read_word(uint16_t addr)
{
    uint16_t word = read_byte(addr) << 8;
    word |= read_byte(addr + 1);
    return word;
}

void write_byte(uint16_t addr, uint8_t data)
{
    RAM[addr] = data;
}

void write_word(uint16_t addr, uint8_t data)
{
    write_byte(addr, (uint8_t)(data >> 8));
    write_byte(addr + 1, (uint8_t)(data & 0xFF));
}

void push_word(uint16_t data)
{
    Stack[SP] = data;
    ++SP;
}

uint16_t pop_word()
{
    --SP;
    return Stack[SP];
}

bool load_rom(const char * filename)
{
    FILE * fp = fopen(filename, "rb");
    if (!fp) {
        fprintf(stderr, "Failed to open ROM '%s'", filename);
        return false;
    }

    fseek(fp, 0, SEEK_END);
    size_t size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    size_t r = fread(RAM + 0x200, 1, size, fp);
    fclose(fp);

    printf("Read %zu bytes\n", r);

    return true;
}

void init_ram()
{
    uint8_t digitSprites[] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80, // F
    };

    memcpy(RAM + DIGIT_SPRITE_OFFSET, digitSprites, sizeof(digitSprites));
}