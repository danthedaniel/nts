#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "console.h"
#include "util.h"
#include "ppu.h"

PPU_t* ppu_init() {
    PPU_t* ppu = (PPU_t*) malloc(sizeof(PPU_t));

    // Zero out memory
    memset(ppu->oam, 0, OAM_SIZE);
    memset(ppu->memory, 0, PPU_MEMORY_SIZE);

    // Set initial register state
    ppu->reg_PPUCTRL   = 0;
    ppu->reg_PPUMASK   = 0;
    ppu->reg_PPUSTATUS = 0b10100000;
    ppu->reg_OAMADDR   = 0;
    ppu->reg_PPUSCROLL = 0;
    ppu->reg_PPUADDR   = 0;
    ppu->reg_PPUDATA   = 0;

    ppu->cycle = 0;
    ppu->scanline = 0;

    return ppu;
}

void ppu_free(PPU_t* ppu) {
    free(ppu);
}

void ppu_tick(PPU_t* ppu) {
    ppu->cycle++;
    // Perform PPU cycle
}

uint8_t* ppu_memory_map_read(PPU_t* ppu, uint16_t address) {
    // The current 8KiB page of CHR ROM is mapped onto the $0000 - $2000 range
    // of the PPU memory map.
    if (address < 0x2000) {
        return &ppu->rom->chr_data[address];
    }

    // The PPU nametables are mapped onto $2000 - $3000.
    if (address >= 0x2000 && address < 0x3000) {
        return ppu_nametable_read(ppu, address - 0x2000);
    }

    // A mirror of $2000 - $2EFF exists in the range $3000 - $3EFF.
    if (address >= 0x3000 && address < 0x3EFF) {
        return ppu_nametable_read(ppu, address - 0x3000);
    }

    // The rest of memory is filled with repeating mirrors of the pallete
    // indices.
    if (address >= 0x3F00 && address < 0x4000) {
        return &ppu->pallette_indices[(address - 0x3F00) % PALLETTE_IND_SIZE];
    }

    return NULL;
}

void ppu_memory_map_write(PPU_t* ppu, uint16_t address, uint8_t value) {
    if (address >= 0x2000 && address < 0x3000) {
        *ppu_nametable_read(ppu, address - 0x2000) = value;
    }

    if (address >= 0x3000 && address < 0x3EFF) {
        *ppu_nametable_read(ppu, address - 0x3000) = value;
    }

    if (address >= 0x3F00 && address < 0x4000) {
        ppu->pallette_indices[(address - 0x3F00) % PALLETTE_IND_SIZE] = value;
    }
}

uint8_t* ppu_nametable_read(PPU_t* ppu, uint16_t address) {
    uint8_t nametable_index = address / (NAMETABLE_SIZE);
    uint8_t relative_addr = address % (NAMETABLE_SIZE);
    // false for vertical, true for horizontal
    bool horizontal = get_bit(ppu->rom->flags6, MIRRORING);

    uint8_t* table1_result = &ppu->memory[relative_addr];
    uint8_t* table2_result = &ppu->memory[(NAMETABLE_SIZE) + relative_addr];

    switch (nametable_index) {
        case 0: return table1_result;
        case 1: return (horizontal ? table1_result : table2_result);
        case 2: return (horizontal ? table2_result : table1_result);
        case 3: return table2_result;
        default: return 0;
    }
}
