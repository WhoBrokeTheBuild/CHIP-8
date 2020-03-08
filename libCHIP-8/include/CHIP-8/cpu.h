#ifndef CPU_H
#define CPU_H

#include <stdint.h>

typedef struct
{
    union {
        struct {
            unsigned aaa:12;
            unsigned op:4;
        };
        struct {
            unsigned n:4;
            unsigned y:4;
            unsigned x:4;
            unsigned :4;
        };
        struct {
            unsigned kk:8;
            unsigned :8;
        };
        uint16_t raw;
    };

} instruction_t;

extern uint8_t V[16];

extern uint16_t I;

extern uint16_t PC;

extern uint16_t SP;

instruction_t fetch();

void execute(instruction_t inst);

#endif // CPU_H