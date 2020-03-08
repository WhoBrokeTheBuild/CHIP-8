#include "memory.h"
#include "cpu.h"

#include <stdio.h>

uint8_t RAM[0xFFF];

uint16_t Stack[16];

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
    Stack[SP++] = data;
}

uint16_t pop_word()
{
    return Stack[--SP];
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