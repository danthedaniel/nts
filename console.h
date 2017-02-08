#ifndef CONSOLE_H__
#define CONSOLE_H__

#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>
#include "rom.h"

#define MASTER_CLOCK 236250000 / 11.0 // NTSC Clock Rate
#define NS_PER_CLOCK (1 / (MASTER_CLOCK)) * 1000000000
#define FRAME_WIDTH 256
#define FRAME_HEIGHT 240

#define SPRITE_SIZE 1 << 2        // 4B
#define OAM_SIZE 1 << 8           // 256B
#define SECONDARY_OAM_SIZE 1 << 5 // 32B
#define PPU_MEMORY_SIZE 1 << 11   // 2KiB
#define PALLETTE_IND_SIZE 0x20    // 32B

#define CPU_MEMORY_SIZE 1 << 11   // 2KiB
#define PAGE_SIZE 1 << 8          // 256B

pthread_mutex_t clock_lock;

typedef struct CPU_t CPU_t;
typedef struct PPU_t PPU_t;
typedef struct APU_t APU_t;

struct CPU_t {
    // REGISTERS
    uint8_t  reg_A;  // Accumulator
    uint8_t  reg_X;  // Index X
    uint8_t  reg_Y;  // Index Y
    uint16_t reg_PC; // Program counter
    uint8_t  reg_S;  // Stack pointer
    uint8_t  reg_P;  // Status register

    // SIGNALS
    bool sig_IRQ;
    bool sig_NMI;

    // MEMORY
    uint8_t memory[CPU_MEMORY_SIZE];

    // OTHER HARDWARE
    ROM_t* cartridge;
    PPU_t* ppu;
    APU_t* apu;

    // CLOCK
    uint64_t cycle; // How many cycles have passed

    // OTHER
    bool powered_on;
};

struct PPU_t {
    // REGISTERS
    uint8_t  reg_PPUCTRL;
    uint8_t  reg_PPUMASK;
    uint8_t  reg_PPUSTATUS;
    uint8_t  reg_OAMADDR;
    uint8_t  reg_OAMDATA;
    uint16_t reg_PPUSCROLL;
    uint16_t reg_PPUADDR;
    uint8_t  reg_PPUDATA;
    uint8_t  reg_OAMDMA;

    // MEMORY
    uint8_t oam[OAM_SIZE];
    uint8_t secondary_oam[SECONDARY_OAM_SIZE];
    uint8_t memory[PPU_MEMORY_SIZE];
    uint8_t pallette_indices[PALLETTE_IND_SIZE];

    // OTHER HARDWARE
    CPU_t* cpu;
    ROM_t* cartridge; // The PPU reads the CHR pages from the ROM

    // CLOCK
    uint64_t cycle;
    uint8_t cycle_budget;

    // RENDERING
    int16_t  scanline;
    uint16_t scanline_cycle;
    uint8_t  framebuffer[FRAME_WIDTH][FRAME_HEIGHT][3];

    // OTHER
    bool mirroring;
};

struct APU_t {
    // PULSE 1
    uint8_t reg_P1DLCV;
    uint8_t reg_P1SWEEP;
    uint8_t reg_P1TIMERLOW;
    uint8_t reg_P1LT;

    // PULSE 2
    uint8_t reg_P2DLCV;
    uint8_t reg_P2SWEEP;
    uint8_t reg_P2TIMERLOW;
    uint8_t reg_P2LT;

    // TRIANGLE
    uint8_t reg_TRLINEARCOUNT;
    uint8_t reg_TRTIMER;
    uint8_t reg_TRLT;

    // NOISE
    uint8_t reg_NLCV;
    uint8_t reg_NLP;
    uint8_t reg_NLENGTH;

    // DMC
    uint8_t reg_DMCILR;
    uint8_t reg_DMCCOUNTER;
    uint8_t reg_DMCADDRESS;
    uint8_t reg_DMCLENGTH;

    // OTHER
    uint8_t reg_APUSTATUS;
    uint8_t reg_FRAMECOUNTER;
};

#endif
