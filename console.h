#include <stdint.h>
#include "rom.h"

#define OAM_SIZE 1<<8
#define CPU_MEMORY_SIZE 2<<11 // 2KiB
#define PPU_MEMORY_SIZE 2<<11 // 2KiB

typedef struct CPU_t CPU_t;
typedef struct PPU_t PPU_t;

struct CPU_t {
    // REGISTERS
    uint8_t  reg_A;  // Accumulator
    uint8_t  reg_X;  // Index X
    uint8_t  reg_Y;  // Index Y
    uint16_t reg_PC; // Program counter
    uint8_t  reg_S;  // Stack pointer
    uint8_t  reg_P;  // Status register

    // MEMORY
    uint8_t memory[CPU_MEMORY_SIZE];

    // OTHER HARDWARE
    ROM_t* cartridge;
    uint8_t cartridge_bank;

    PPU_t* ppu;
};

struct PPU_t {
    // REGISTERS
    uint8_t reg_PPUCTRL;
    uint8_t reg_PPUMASK;
    uint8_t reg_PPUSTATUS;
    uint8_t reg_OAMADDR;
    uint8_t reg_OAMDATA;
    uint8_t reg_PPUSCROLL;
    uint8_t reg_PPUADDR;
    uint8_t reg_PPUDATA;
    uint8_t reg_OAMDMA;

    // MEMORY
    uint8_t oam[OAM_SIZE];
    uint8_t memory[PPU_MEMORY_SIZE];

    // OTHER HARDWARE
    CPU_t* cpu;
};
