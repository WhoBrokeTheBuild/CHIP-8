#include "cpu.h"
#include "memory.h"

#include <stdio.h>

uint8_t V[16];

uint16_t I;

uint16_t PC;

uint8_t SP;

typedef void(* instruction_func_t)(instruction_t);

instruction_t fetch()
{
    instruction_t inst = { 
        .raw = read_word(PC),
    };
    PC += 2;
    return inst;
}

void CLS(instruction_t inst)
{
    printf("CLS\n");

}

void RET(instruction_t inst)
{
    printf("RET\n");
    PC = pop_word();
}

void opcode_0(instruction_t inst)
{
    if (inst.kk == 0xE0) {
        CLS(inst);
    }
    else if (inst.kk == 0xEE) {
        RET(inst);
    }
    else {
        fprintf(stderr, "'SYS': Instruction Not Implemented\n");
    }
}

void JP_aaa(instruction_t inst)
{
    printf("JP %03X\n", inst.aaa);
    PC = inst.aaa;
}

void CALL_aaa(instruction_t inst)
{
    printf("CALL %03X\n", inst.aaa);
    push_word(PC);
    PC = inst.aaa;
}

void SE_Vx_kk(instruction_t inst)
{
    printf("SE V%X %02X", inst.x, inst.kk);
    if (V[inst.x] == inst.kk) {
        PC += 2;
    }
}

void SNE_Vx_kk(instruction_t inst)
{
    printf("SNE V%X %02X", inst.x, inst.kk);
    if (V[inst.x] != inst.kk) {
        PC += 2;
    }
}

void SE_Vx_Vy(instruction_t inst)
{
    printf("SE V%X V%X", inst.x, inst.y);
    if (V[inst.x] == V[inst.y]) {
        PC += 2;
    }
}

void LD_Vx_kk(instruction_t inst)
{
    printf("LD V%X %02X\n", inst.x, inst.kk);
    V[inst.x] = inst.kk;
}

void ADD_Vx_kk(instruction_t inst)
{
    printf("ADD V%X %02X\n", inst.x, inst.kk);
    int r = V[inst.x] + inst.kk;
    if (r > 0xFF) {
        V[0xF] = 1;
    }
    V[inst.x] = r;
}

void LD_Vx_Vy(instruction_t inst)
{
    printf("LD V%X V%X\n", inst.x, inst.y);
    V[inst.x] = V[inst.y];
}

void OR_Vx_Vy(instruction_t inst)
{
    printf("OR V%X V%X\n", inst.x, inst.y);
    V[inst.x] |= V[inst.y];
}

void AND_Vx_Vy(instruction_t inst)
{
    printf("AND V%X V%X\n", inst.x, inst.y);
    V[inst.x] &= V[inst.y];
}

void XOR_Vx_Vy(instruction_t inst)
{
    printf("XOR V%X V%X\n", inst.x, inst.y);
    V[inst.x] ^= V[inst.y];
}

void ADD_Vx_Vy(instruction_t inst)
{
    printf("ADD V%X V%X\n", inst.x, inst.y);
    int r = V[inst.x] + V[inst.y];
    if (r > 0xFF) {
        V[0xF] = 1;
    }
    V[inst.x] = r;
}

void SUB_Vx_Vy(instruction_t inst)
{
    printf("SUB V%X V%X\n", inst.x, inst.y);
    V[0xF] = (V[inst.x] > V[inst.y] ? 1 : 0);
    V[inst.x] -= V[inst.y];
}

void SHR_Vx_Vy(instruction_t inst)
{
    printf("SHR V%X V%X\n", inst.x, inst.y);
    V[0xF] = (V[inst.y] & 0x01);
    V[inst.x] = (V[inst.y] >> 1);
}

void SUBN_Vx_Vy(instruction_t inst)
{
    printf("SUBN V%X V%X\n", inst.x, inst.y);
    V[0xF] = (V[inst.y] > V[inst.x] ? 1 : 0);
    V[inst.x] -= V[inst.y];
}

void SHL_Vx_Vy(instruction_t inst)
{
    printf("SHL V%X V%X\n", inst.x, inst.y);
    V[0xF] = (V[inst.y] & 0x80);
    V[inst.x] = (V[inst.y] << 1);
}

void opcode_8(instruction_t inst)
{
    static instruction_func_t map[16] = {
        [0x0] = LD_Vx_Vy,
        [0x1] = OR_Vx_Vy,
        [0x2] = AND_Vx_Vy,
        [0x3] = XOR_Vx_Vy,
        [0x4] = ADD_Vx_Vy,
        [0x5] = SUB_Vx_Vy,
        [0x6] = SHR_Vx_Vy,
        [0x7] = SUBN_Vx_Vy,
        [0x8] = NULL,
        [0x9] = NULL,
        [0xA] = NULL,
        [0xB] = NULL,
        [0xC] = NULL,
        [0xD] = NULL,
        [0xE] = SHL_Vx_Vy,
        [0xF] = NULL,
    };

    if (map[inst.n]) {
        map[inst.n](inst);
    }
}

void SNE_Vx_Vy(instruction_t inst)
{
    printf("SNE V%X V%X", inst.x, inst.y);
    if (V[inst.x] != V[inst.y]) {
        PC += 2;
    }
}

void LD_I_aaa(instruction_t inst)
{
    printf("LD I %03X\n", inst.aaa);
    I = inst.aaa;
}

void JP_V0_aaa(instruction_t inst)
{
    PC = inst.aaa + V[0];
}

void RND_Vx_kk(instruction_t inst)
{
    V[inst.x] = (rand() % 255) & inst.kk;
}

void DRW_Vx_Vy_kk(instruction_t inst) 
{

}

void SKP_Vx(instruction_t inst)
{

}

void SKNP_Vx(instruction_t inst)
{
    
}

void opcode_E(instruction_t inst)
{
    if (inst.kk == 0x9E) {
        SKP_Vx(inst);
    }
    else if (inst.kk == 0xA1) {
        SKNP_Vx(inst);
    }
    else {
        fprintf(stderr, "Invalid Instruction\n");
    }
}

void opcode_F(instruction_t inst)
{
    if (inst.kk == 0x07) {
    }
    else if (inst.kk == 0x0A) {
    }
    else if (inst.kk == 0x15) {
    }
    else if (inst.kk == 0x18) {
    }
    else if (inst.kk == 0x1E) {
    }
    else if (inst.kk == 0x29) {
    }
    else if (inst.kk == 0x33) {
    }
    else if (inst.kk == 0x55) {
    }
    else if (inst.kk == 0x65) {
    }
    else {
        fprintf(stderr, "Invalid Instruction\n");
    }
}

void execute(instruction_t inst)
{
    static instruction_func_t map[16] = 
    {
        [0x0] = opcode_0,
        [0x1] = JP_aaa,
        [0x2] = CALL_aaa,
        [0x3] = SE_Vx_kk,
        [0x4] = SNE_Vx_kk,
        [0x5] = SE_Vx_Vy,
        [0x6] = LD_Vx_kk,
        [0x7] = ADD_Vx_kk,
        [0x8] = opcode_8,
        [0x9] = SNE_Vx_Vy,
        [0xA] = LD_I_aaa,
        // [0xB] = JP_V0_aaa,
        // [0xC] = RND_Vx_kk,
        // [0xD] = DRW_Vx_Vy_kk,
        [0xE] = opcode_E,
        [0xF] = opcode_F,
    };

    printf("%03X: raw=%04X opcode=%X addr=%03X x=%X y=%X n=%X kk=%02X\n", 
        PC-2, inst.raw, inst.op, inst.aaa, inst.x, inst.y, inst.n, inst.kk);

    if (map[inst.op]) {
        map[inst.op](inst);
    }
}