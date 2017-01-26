#ifndef PPU_H__
#define PPU_H__

#include <stdint.h>
#include "console.h"

PPU_t* ppu_init();
void ppu_free();

uint8_t ppu_memory_map_read(PPU_t* ppu, uint16_t address);
void ppu_memory_map_write(PPU_t* ppu, uint16_t address, uint8_t value);

#endif
