#ifndef PPU_H__
#define PPU_H__

#include <stdint.h>
#include "rom.h"
#include "console.h"

#define PPU_CLOCK (MASTER_CLOCK) / 4.0
#define NUM_SCANELINES 262
#define CYCLES_PER_SCANLINE 341
#define TILE_SIZE 1 << 3 // 8 pixels
#define TILES_PER_SCANLINE (FRAME_WIDTH) / (TILE_SIZE)
#define NAMETABLE_SIZE 1 << 10 // 1KiB

PPU_t* ppu_init(ROM_t* cartridge);
void ppu_free(PPU_t* ppu);

void ppu_start(PPU_t* ppu);
void ppu_tick(PPU_t* ppu);

// Rendering functions
void ppu_render_scanline(PPU_t* ppu);
void ppu_prerender_scanline(PPU_t* ppu);
void ppu_visible_scanline(PPU_t* ppu);
void ppu_idle_scanline(PPU_t* ppu);
void ppu_vblank_scanline(PPU_t* ppu);
void ppu_sprite_eval(PPU_t* ppu);

// Memory functions
uint8_t* ppu_memory_map_read(PPU_t* ppu, uint16_t address);
uint8_t* ppu_nametable_read(PPU_t* ppu, uint16_t address);
void ppu_memory_map_write(PPU_t* ppu, uint16_t address, uint8_t value);
void ppu_fake_memory_access(PPU_t* ppu);

// OAM functions
uint8_t* ppu_read_oam(PPU_t* ppu, uint8_t address);
void ppu_write_oam_from_reg(PPU_t* ppu);
void ppu_write_from_reg(PPU_t* ppu);
uint16_t sprite_pattern_address(uint8_t* sprite);

enum PPUStatusBits {
    stat_VBLANK     = 7,
    stat_SPRITE0    = 6,
    stat_SPRITEOVER = 5,
    reg_LSB         = 0b00001111
};

// NES pallette in 24-bit RGB
uint8_t PALLETTE[56][3] = {
    {84,  84,  84 },
    {0,   30,  116},
    {8,   16,  144},
    {48,  0,   136},
    {68,  0,   100},
    {92,  0,   48 },
    {84,  4,   0  },
    {60,  24,  0  },
    {32,  42,  0  },
    {8,   58,  0  },
    {0,   64,  0  },
    {0,   60,  0  },
    {0,   50,  60 },
    {0,   0,   0  },
    {152, 150, 152},
    {8,   76,  196},
    {48,  50,  236},
    {92,  30,  228},
    {136, 20,  176},
    {160, 20,  100},
    {152, 34,  32 },
    {120, 60,  0  },
    {84,  90,  0  },
    {40,  114, 0  },
    {8,   124, 0  },
    {0,   118, 40 },
    {0,   102, 120},
    {0,   0,   0  },
    {236, 238, 236},
    {76,  154, 236},
    {120, 124, 236},
    {176, 98,  236},
    {228, 84,  236},
    {236, 88,  180},
    {236, 106, 100},
    {212, 136, 32 },
    {160, 170, 0  },
    {116, 196, 0  },
    {76,  208, 32 },
    {56,  204, 108},
    {56,  180, 204},
    {60,  60,  60 },
    {236, 238, 236},
    {168, 204, 236},
    {188, 188, 236},
    {212, 178, 236},
    {236, 174, 236},
    {236, 174, 212},
    {236, 180, 176},
    {228, 196, 144},
    {204, 210, 120},
    {180, 222, 120},
    {168, 226, 144},
    {152, 226, 180},
    {160, 214, 228},
    {160, 162, 160}
};

#endif
