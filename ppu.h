#ifndef PPU_H__
#define PPU_H__

#include <stdint.h>
#include "console.h"

#define PPU_CLOCK (MASTER_CLOCK) / 4.0

PPU_t* ppu_init();
void ppu_free(PPU_t* ppu);

void ppu_tick(PPU_t* ppu);

uint8_t ppu_memory_map_read(PPU_t* ppu, uint16_t address);
void ppu_memory_map_write(PPU_t* ppu, uint16_t address, uint8_t value);

#endif
