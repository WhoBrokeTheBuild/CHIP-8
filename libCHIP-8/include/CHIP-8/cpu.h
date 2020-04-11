#ifndef CPU_H
#define CPU_H

#include "macros.h"

#include <assert.h>
#include <stdint.h>

typedef union
{
    PACK(struct
    {
        uint16_t aaa:12;
        uint16_t op:4;
    });

    PACK(struct
    {
        uint16_t n:4;
        uint16_t y:4;
        uint16_t x:4;
        uint16_t :4;
    });

    PACK(struct
    {
        uint16_t kk:8;
        uint16_t :8;
    });

    uint16_t raw;

} instruction_t;

static_assert(sizeof(instruction_t) == 2,
    "sizeof(instruction_t) != 2");

extern uint8_t V[16];

extern uint16_t I;

extern uint16_t PC;

extern uint16_t SP;

instruction_t fetch();

void execute(instruction_t inst);

#endif // CPU_H