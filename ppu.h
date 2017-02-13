#ifndef PPU_H__
#define PPU_H__

#include <stdint.h>
#include <stdbool.h>
#include "rom.h"
#include "console.h"

#define PPU_CLOCK           (MASTER_CLOCK) / 4.0
#define NUM_SCANELINES      262
#define CYCLES_PER_SCANLINE 341
#define TILE_SIZE           1 << 3  // 8 pixels
#define TILES_PER_SCANLINE  (FRAME_WIDTH) / (TILE_SIZE)
#define NAMETABLE_SIZE      1 << 10 // 1KiB
#define RENDERING_MASK      0b00011000

PPU_t* ppu_init(ROM_t* cartridge);
void ppu_free(PPU_t* ppu);

// Cycle instructions
void ppu_start(PPU_t* ppu);
void ppu_tick(PPU_t* ppu);

// Rendering functions
void ppu_render_scanline(PPU_t* ppu);
void ppu_prerender_scanline(PPU_t* ppu);
void ppu_visible_scanline(PPU_t* ppu);
void ppu_idle_scanline(PPU_t* ppu);
void ppu_vblank_scanline(PPU_t* ppu);
void ppu_sprite_eval(PPU_t* ppu);
uint8_t ppu_get_pallette(PPU_t* ppu, bool sprite, uint8_t num, uint8_t value);

// Helper functions
bool ppu_rendering_enabled(PPU_t* ppu);
uint16_t ppu_base_nametable(PPU_t* ppu);
uint16_t ppu_base_patterntable(PPU_t* ppu);
uint8_t ppu_vram_inc(PPU_t* ppu);
const uint8_t* ppu_rgb_from_pallette(PPU_t* ppu, uint8_t i);

// Memory functions
uint8_t* ppu_memory_map_read(PPU_t* ppu, uint16_t address);
uint8_t* ppu_memory_map_read_inc(PPU_t* ppu, uint16_t address);
uint8_t* ppu_nametable_read(PPU_t* ppu, uint16_t address);
void ppu_memory_map_write(PPU_t* ppu, uint16_t address, uint8_t value);
void ppu_memory_map_write_inc(PPU_t* ppu, uint16_t address, uint8_t value);
void ppu_fake_memory_access(PPU_t* ppu);
uint8_t* ppu_read_oam_from_reg(PPU_t* ppu, uint8_t i);

// OAM functions
uint8_t* ppu_read_oam(PPU_t* ppu, uint8_t address);
void ppu_write_oam_from_reg(PPU_t* ppu);
uint16_t sprite_pattern_address(uint8_t* sprite);

enum PPUStatusBits {
    stat_VBLANK     = 7,
    stat_SPRITE0    = 6,
    stat_SPRITEOVER = 5,
    reg_LSB         = 0b00011111
};

enum PPUMaskBits {
    mask_BLUE         = 7,
    mask_GREEN        = 6,
    mask_RED          = 5,
    mask_SPRITES      = 4,
    mask_BG           = 3,
    mask_LEFTSPRITES  = 2,
    mask_RIGHTSPRITES = 1,
    mask_GRAYSCALE    = 0,
};

enum PPUCtrlBits {
    ctrl_NMI         = 7,
    ctrl_PPUMASTER   = 6,
    ctrl_SPRITESIZE  = 5,
    ctrl_BGPTABLE    = 4,
    ctrl_SPRITETABLE = 3,
    ctrl_VRAMINC     = 2,
    ctrl_BASENTABLE  = 0b00000011
};

// NES reference pallette in 24-bit RGB
const uint8_t REF_PALLETTE_MAP[64][3] = {
    {0x7C, 0x7C, 0x7C}, // #7C7C7C
    {0x00, 0x00, 0xFC}, // #0000FC
    {0x00, 0x00, 0xBC}, // #0000BC
    {0x44, 0x28, 0xBC}, // #4428BC
    {0x94, 0x00, 0x84}, // #940084
    {0xA8, 0x00, 0x20}, // #A80020
    {0xA8, 0x10, 0x00}, // #A81000
    {0x88, 0x14, 0x00}, // #881400
    {0x50, 0x30, 0x00}, // #503000
    {0x00, 0x78, 0x00}, // #007800
    {0x00, 0x68, 0x00}, // #006800
    {0x00, 0x58, 0x00}, // #005800
    {0x00, 0x40, 0x58}, // #004058
    {0x00, 0x00, 0x00}, // #000000
    {0x00, 0x00, 0x00}, // #000000
    {0x00, 0x00, 0x00}, // #000000
    {0xBC, 0xBC, 0xBC}, // #BCBCBC
    {0x00, 0x78, 0xF8}, // #0078F8
    {0x00, 0x58, 0xF8}, // #0058F8
    {0x68, 0x44, 0xFC}, // #6844FC
    {0xD8, 0x00, 0xCC}, // #D800CC
    {0xE4, 0x00, 0x58}, // #E40058
    {0xF8, 0x38, 0x00}, // #F83800
    {0xE4, 0x5C, 0x10}, // #E45C10
    {0xAC, 0x7C, 0x00}, // #AC7C00
    {0x00, 0xB8, 0x00}, // #00B800
    {0x00, 0xA8, 0x00}, // #00A800
    {0x00, 0xA8, 0x44}, // #00A844
    {0x00, 0x88, 0x88}, // #008888
    {0x00, 0x00, 0x00}, // #000000
    {0x00, 0x00, 0x00}, // #000000
    {0x00, 0x00, 0x00}, // #000000
    {0xF8, 0xF8, 0xF8}, // #F8F8F8
    {0x3C, 0xBC, 0xFC}, // #3CBCFC
    {0x68, 0x88, 0xFC}, // #6888FC
    {0x98, 0x78, 0xF8}, // #9878F8
    {0xF8, 0x78, 0xF8}, // #F878F8
    {0xF8, 0x58, 0x98}, // #F85898
    {0xF8, 0x78, 0x58}, // #F87858
    {0xFC, 0xA0, 0x44}, // #FCA044
    {0xF8, 0xB8, 0x00}, // #F8B800
    {0xB8, 0xF8, 0x18}, // #B8F818
    {0x58, 0xD8, 0x54}, // #58D854
    {0x58, 0xF8, 0x98}, // #58F898
    {0x00, 0xE8, 0xD8}, // #00E8D8
    {0x78, 0x78, 0x78}, // #787878
    {0x00, 0x00, 0x00}, // #000000
    {0x00, 0x00, 0x00}, // #000000
    {0xFC, 0xFC, 0xFC}, // #FCFCFC
    {0xA4, 0xE4, 0xFC}, // #A4E4FC
    {0xB8, 0xB8, 0xF8}, // #B8B8F8
    {0xD8, 0xB8, 0xF8}, // #D8B8F8
    {0xF8, 0xB8, 0xF8}, // #F8B8F8
    {0xF8, 0xA4, 0xC0}, // #F8A4C0
    {0xF0, 0xD0, 0xB0}, // #F0D0B0
    {0xFC, 0xE0, 0xA8}, // #FCE0A8
    {0xF8, 0xD8, 0x78}, // #F8D878
    {0xD8, 0xF8, 0x78}, // #D8F878
    {0xB8, 0xF8, 0xB8}, // #B8F8B8
    {0xB8, 0xF8, 0xD8}, // #B8F8D8
    {0x00, 0xFC, 0xFC}, // #00FCFC
    {0xF8, 0xD8, 0xF8}, // #F8D8F8
    {0x00, 0x00, 0x00}, // #000000
    {0x00, 0x00, 0x00}, // #000000
};

#endif
