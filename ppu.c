#include <stdint.h>
#include <stdlib.h>
#include <string.h>
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

    return ppu;
}

void ppu_free(PPU_t* ppu) {
    free(ppu);
}

uint8_t ppu_memory_map_read(PPU_t* ppu, uint16_t address) {
    return 0;
}
