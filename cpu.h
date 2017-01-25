#ifndef __CPU_H__
#define __CPU_H__

#include <stdint.h>
#include "rom.h"

#define MEMORY_SIZE 2<<11 // 2KiB

typedef struct {
    // REGISTERS
    uint8_t  reg_A;  // Accumulator
    uint8_t  reg_X;  // Index X
    uint8_t  reg_Y;  // Index Y
    uint16_t reg_PC; // Program counter
    uint8_t  reg_S;  // Stack pointer
    uint8_t  reg_P;  // Status register

    // MEMORY
    uint8_t memory[MEMORY_SIZE];

    // PERIPHERALS
    ROM_t* cartridge;
    uint8_t cartridge_bank;
} CPU_t;

CPU_t* cpu_init();
void cpu_clock(CPU_t* cpu);
uint8_t cpu_memory_map_read(CPU_t* cpu, uint16_t address);
void cpu_free();

#endif
