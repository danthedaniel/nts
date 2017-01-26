#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "ppu.h"

PPU_t* ppu_init() {
    PPU_t* ppu = (PPU_t*) malloc(sizeof(PPU_t));

    // Zero out memory
    memset(ppu->oam, 0, OAM_SIZE);
    memset(ppu->memory, 0, PPU_MEMORY_SIZE);

    return ppu;
}

void ppu_free(PPU_t* ppu) {
    free(ppu);
}

uint8_t ppu_memory_map_read(PPU_t* ppu, uint16_t address) {
    return 0;
}
