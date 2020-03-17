#include "cpu.h"
#include "input.h"
#include "memory.h"
#include "timer.h"
#include "video.h"

#include <stdio.h>
#include <stdlib.h>

uint8_t V[16] = { 0 };

uint16_t I = 0x000;

uint16_t PC = 0x200;

uint16_t SP = 0;

typedef void(* instruction_func_t)(instruction_t);

void CLS(instruction_t inst)
{
    // printf("CLS\n");
    for (int y = 0; y < SCREEN_HEIGHT; ++y) {
        for (int x = 0; x < SCREEN_WIDTH; ++x) {
            VRAM[y][x] = 0;
        }
    }
}

void RET(instruction_t inst)
{
    // printf("RET\n");
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
        // fprintf(stderr, "'SYS': Instruction Not Implemented\n");
    }
}

void JP_aaa(instruction_t inst)
{
    // printf("JP %03X\n", inst.aaa);
    PC = inst.aaa;
}

void CALL_aaa(instruction_t inst)
{
    // printf("CALL %03X\n", inst.aaa);
    push_word(PC);
    PC = inst.aaa;
}

void SE_Vx_kk(instruction_t inst)
{
    // printf("SE V%X %02X", inst.x, inst.kk);
    if (V[inst.x] == inst.kk) {
        PC += 2;
    }
}

void SNE_Vx_kk(instruction_t inst)
{
    // printf("SNE V%X %02X", inst.x, inst.kk);
    if (V[inst.x] != inst.kk) {
        PC += 2;
    }
}

void SE_Vx_Vy(instruction_t inst)
{
    // printf("SE V%X V%X", inst.x, inst.y);
    if (V[inst.x] == V[inst.y]) {
        PC += 2;
    }
}

void LD_Vx_kk(instruction_t inst)
{
    // printf("LD V%X %02X\n", inst.x, inst.kk);
    V[inst.x] = inst.kk;
}

void ADD_Vx_kk(instruction_t inst)
{
    // printf("ADD V%X %02X\n", inst.x, inst.kk);
    int r = V[inst.x] + inst.kk;
    V[0xF] = (r > 0xFF ? 1 : 0);
    V[inst.x] = r;
}

void LD_Vx_Vy(instruction_t inst)
{
    // printf("LD V%X V%X\n", inst.x, inst.y);
    V[inst.x] = V[inst.y];
}

void OR_Vx_Vy(instruction_t inst)
{
    // printf("OR V%X V%X\n", inst.x, inst.y);
    V[inst.x] |= V[inst.y];
}

void AND_Vx_Vy(instruction_t inst)
{
    // printf("AND V%X V%X\n", inst.x, inst.y);
    V[inst.x] &= V[inst.y];
}

void XOR_Vx_Vy(instruction_t inst)
{
    // printf("XOR V%X V%X\n", inst.x, inst.y);
    V[inst.x] ^= V[inst.y];
}

void ADD_Vx_Vy(instruction_t inst)
{
    // printf("ADD V%X V%X\n", inst.x, inst.y);
    int r = V[inst.x] + V[inst.y];
    V[0xF] = (r > 0xFF ? 1 : 0);
    V[inst.x] = r;
}

void SUB_Vx_Vy(instruction_t inst)
{
    // printf("SUB V%X V%X\n", inst.x, inst.y);
    V[0xF] = (V[inst.y] > V[inst.x] ? 1 : 0);
    V[inst.x] -= V[inst.y];
}

void SHR_Vx_Vy(instruction_t inst)
{
    // printf("SHR V%X V%X\n", inst.x, inst.y);
    V[0xF] = (V[inst.y] & 0x01);
    V[inst.x] = (V[inst.y] >> 1);
}

void SUBN_Vx_Vy(instruction_t inst)
{
    // printf("SUBN V%X V%X\n", inst.x, inst.y);
    V[0xF] = (V[inst.y] > V[inst.x] ? 0 : 1);
    V[inst.x] -= V[inst.y];
}

void SHL_Vx_Vy(instruction_t inst)
{
    // printf("SHL V%X V%X\n", inst.x, inst.y);
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
    // printf("SNE V%X V%X\n", inst.x, inst.y);
    if (V[inst.x] != V[inst.y]) {
        PC += 2;
    }
}

void LD_I_aaa(instruction_t inst)
{
    // printf("LD I %03X\n", inst.aaa);
    I = inst.aaa;
}

void JP_V0_aaa(instruction_t inst)
{
    // printf("JP V0 %03X\n", inst.aaa);
    PC = inst.aaa + V[0];
}

void RND_Vx_kk(instruction_t inst)
{
    // printf("JP V%X %02X\n", inst.x, inst.kk);
    V[inst.x] = (rand() % 256) & inst.kk;
}

void DRW_Vx_Vy_kk(instruction_t inst) 
{
    printf("DRW V%X V%X %d\n", inst.x, inst.y, inst.n);
    V[0xF] = 0;
    for (int i = 0; i < inst.n; ++i) {
        uint8_t data = RAM[I + i];
        for (int j = 0; j < 8; ++j) {
            int x = (V[inst.x] + j) % SCREEN_WIDTH;
            int y = (V[inst.y] + i) % SCREEN_HEIGHT;
            int pixel = (data & (0x80 >> j));
            if (VRAM[y][x] > 0 && pixel > 0) {
                V[0xF] = 1;
            }
            VRAM[y][x] ^= (pixel > 0 ? 0xFF : 0x00);
        }
    }
}

void SKP_Vx(instruction_t inst)
{
    // printf("SKP V%X\n", inst.x);
    if (Keys[V[inst.x]]) {
        Keys[V[inst.x]] = false;
        PC += 2;
    }
}

void SKNP_Vx(instruction_t inst)
{
    // printf("SKNP V%X\n", inst.x);
    if (!Keys[V[inst.x]]) {
        PC += 2;
    }
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
        // fprintf(stderr, "Invalid Instruction\n");
    }
}

void LD_Vx_DT(instruction_t inst)
{
    // printf("LD V%X DT\n", inst.x);
    V[inst.x] = DT;
}

void LD_Vx_K(instruction_t inst)
{
    // printf("LD V%X K\n", inst.x);
    WaitInputVx = inst.x;
}

void LD_DT_Vx(instruction_t inst)
{
    // printf("LD DT V%X\n", inst.x);
    DT = V[inst.x];
}

void LD_ST_Vx(instruction_t inst)
{
    // printf("LD ST V%X\n", inst.x);
    ST = V[inst.x];
}

void ADD_I_Vx(instruction_t inst)
{
    // printf("LD I V%X\n", inst.x);
    I += V[inst.x];
}

void LD_F_Vx(instruction_t inst)
{
    // printf("LD F V%X\n", inst.x);
    I = DIGIT_SPRITE_OFFSET + (V[inst.x] * BYTES_PER_DIGIT);
}

void LD_B_Vx(instruction_t inst)
{
    // printf("LD B V%X\n", inst.x);
    RAM[I + 0] = V[inst.x] / 100;
    RAM[I + 1] = (V[inst.x] % 100) / 10;
    RAM[I + 2] = V[inst.x] % 10;
}

void LD_pI_Vx(instruction_t inst)
{
    // printf("LD (I) V%X\n", inst.x);
    for (int i = 0; i <= inst.x; ++i) {
        write_byte(I + i, V[i]);
    }
}

void LD_Vx_pI(instruction_t inst)
{
    // printf("LD V%X (I)\n", inst.x);
    for (int i = 0; i <= inst.x; ++i) {
        V[i] = read_byte(I + i);
    }
}

void opcode_F(instruction_t inst)
{
    if (inst.kk == 0x07) {
        LD_Vx_DT(inst);
    }
    else if (inst.kk == 0x0A) {
        LD_Vx_K(inst);
    }
    else if (inst.kk == 0x15) {
        LD_DT_Vx(inst);
    }
    else if (inst.kk == 0x18) {
        LD_ST_Vx(inst);
    }
    else if (inst.kk == 0x1E) {
        ADD_I_Vx(inst);
    }
    else if (inst.kk == 0x29) {
        LD_F_Vx(inst);
    }
    else if (inst.kk == 0x33) {
        LD_B_Vx(inst);
    }
    else if (inst.kk == 0x55) {
        LD_pI_Vx(inst);
    }
    else if (inst.kk == 0x65) {
        LD_Vx_pI(inst);
    }
    else {
        fprintf(stderr, "Invalid Instruction\n");
    }
}

instruction_t fetch()
{
    instruction_t inst = { 
        .raw = read_word(PC),
    };
    // printf("[%03X] OP=%X AAA=%03X X=%X Y=%X KK=%02X\n", PC, inst.op, inst.aaa, inst.x, inst.y, inst.kk);
    PC += 2;
    return inst;
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
        [0xB] = JP_V0_aaa,
        [0xC] = RND_Vx_kk,
        [0xD] = DRW_Vx_Vy_kk,
        [0xE] = opcode_E,
        [0xF] = opcode_F,
    };

    if (map[inst.op]) {
        map[inst.op](inst);
    }
}