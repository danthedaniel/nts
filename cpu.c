#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "cpu.h"
#include "rom.h"

CPU_t* cpu_init() {
    CPU_t* cpu = (CPU_t*) malloc(sizeof(CPU_t));

    // Zero out system memory
    memset(cpu->memory, 0, MEMORY_SIZE);

    // Set up registers
    cpu->reg_A = 0;
    cpu->reg_X = 0;
    cpu->reg_Y = 0;
    cpu->reg_P = 0x34;
    cpu->reg_S = 0xFD;
    cpu->reg_PC = 0; // TODO: Confirm this

    return cpu;
}

void cpu_free(CPU_t* cpu) {
    free(cpu);
}

uint8_t cpu_memory_map_read(CPU_t* cpu, uint16_t address) {
    // The 2KiB of system memory is mapped from $0000-$07FF, but it's also
    // mirrored to $0800-$1FFF 3 times
    if (address < 0x2000) {
        return cpu->memory[address % MEMORY_SIZE];
    }

    // The PPU's 8 registers are mapped onto $2000-$2007, and mirrored through
    // $3FFF (so they repeat every 8 bytes)
    if (address >= 0x2000 && address < 0x4000) {
        switch(address % 8) {
            case 0:
                // PPUCTRL register
                return 0;
                break;
            case 1:
                // PPUMASK register
                return 0;
                break;
            case 2:
                // PPUSTATUS register
                return 0;
                break;
            case 3:
                // OAMADDR register
                return 0;
                break;
            case 4:
                // OAMDATA register
                return 0;
                break;
            case 5:
                // PPUSCROLL register
                return 0;
                break;
            case 6:
                // PPUADDR register
                return 0;
                break;
            case 7:
                // PPUDATA register
                return 0;
                break;
        }
    }

    // NES APU and I/O registers
    if (address >= 0x4000 && address < 0x4017) {
        return 0;
    }

    // APU and I/O functionality that is usually disabled
    if (address >= 0x4018 && address < 0x401F) {
        return 0;
    }

    // Expansion RAM
    if (address >= 0x4020 && address < 0x5FFF) {
        return 0;
    }

    // Cartridge SRAM
    if (address >= 0x6000 && address < 0x7FFF) {
        return 0;
    }

    // Cartridge PrgROM
    if (address >= 0x8000 && address < 0xFFFF) {
        uint32_t prg_offset = (cpu->cartridge_bank * (PRG_PAGE_SIZE));
        return cpu->cartridge->prg_data[prg_offset + (address - 0x8000)];
    }

    return 0;
}
