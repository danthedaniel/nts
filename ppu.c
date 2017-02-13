#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "ppu.h"
#include "util.h"

PPU_t* ppu_init(ROM_t* cartridge) {
    PPU_t* ppu = (PPU_t*) malloc(sizeof(PPU_t));

    // Zero out memory
    memset(ppu->oam, 0, OAM_SIZE);
    memset(ppu->secondary_oam, 0, SECONDARY_OAM_SIZE);
    memset(ppu->memory, 0, PPU_MEMORY_SIZE);
    memset(ppu->pallette_indices, 0, PALLETTE_IND_SIZE);

    // Set initial register state
    ppu->reg_PPUCTRL   = 0;
    ppu->reg_PPUMASK   = 0;
    ppu->reg_PPUSTATUS = 0b10100000;
    ppu->reg_OAMADDR   = 0;
    ppu->reg_PPUSCROLL = 0;
    ppu->reg_PPUADDR   = 0;
    ppu->reg_PPUDATA   = 0;

    ppu->address_latch = false;
    ppu->clear_vsync = false;

    ppu->framenumber = 0;
    ppu->cycle       = 0;
    ppu->scanline    = -1;
    ppu->cartridge   = cartridge;
    // false for vertical, true for horizontal
    ppu->mirroring   = get_bit(ppu->cartridge->flags6, MIRRORING);

    return ppu;
}

void ppu_free(PPU_t* ppu) {
    free(ppu);
}

// Cycle instructions
void ppu_start(PPU_t* ppu) {
    pthread_mutex_lock(&clock_lock);

    while (ppu->cpu->powered_on) {
        ppu_render_scanline(ppu);
    }

    pthread_mutex_unlock(&clock_lock);
}

void ppu_tick(PPU_t* ppu) {
    ppu->cycle++;
    ppu->scanline_cycle++;

    // Because the PPU runs at 3x the clockspeed of the CPU, we give the CPU a
    // clock cycle every 3 PPU cycles.
    if (ppu->cycle % 3 == 0)
        ppu->cpu->cycle_budget++;

    if (ppu->clear_vsync) {
        ppu->reg_PPUSTATUS = set_bit(ppu->reg_PPUSTATUS, stat_VBLANK, false);
        ppu->clear_vsync = false;
    }

    while (ppu->cycle_budget == 0) {
        pthread_mutex_unlock(&clock_lock);
        pthread_mutex_lock(&clock_lock);
    }

    ppu->cycle_budget--;
}

// Rendering functions
void ppu_render_scanline(PPU_t* ppu) {
    ppu->scanline_cycle = 0;

    if (ppu->scanline == -1 || ppu->scanline == 261)
        ppu_prerender_scanline(ppu);
    else if (ppu->scanline >= 0 && ppu->scanline < 240)
        ppu_visible_scanline(ppu);
    else if (ppu->scanline == 240)
        ppu_idle_scanline(ppu);
    else if (ppu->scanline == 241)
        ppu_vblank_scanline(ppu);
    else if (ppu->scanline >= 242 && ppu->scanline < 261)
        ppu_idle_scanline(ppu);

    ppu->scanline++;
    if (ppu->scanline > 261) {
        ppu->framenumber++;
        ppu->scanline = 0;
    }
}

void ppu_prerender_scanline(PPU_t* ppu) {
    ppu->reg_PPUSTATUS = set_bit(ppu->reg_PPUSTATUS, stat_VBLANK, false);

    // TODO: Set OAMADDR to 0 during ticks 257-320 (sprite tile loading interval)
}

void ppu_visible_scanline(PPU_t* ppu) {
    // On odd frames the first scanline is one cycle shorter as long as
    // rendering is enabled
    if (!(ppu_rendering_enabled(ppu) &&
        ppu->scanline == 0 &&
        ppu->framenumber % 2 == 1)) {
        ppu_tick(ppu);
    }

    // Read tile data
    for (int i = 0; i < TILES_PER_SCANLINE; ++i) {
        // TODO:
        // Read nametable byte
        // Read Attribute table byte
        // Read tile bitmap low
        // Read tile bitmap high (at low addr + 8)
    }

    // Preload sprites for the next scanline
    for (int i = 0; i < ((SECONDARY_OAM_SIZE) / (SPRITE_SIZE)); ++i) {
        // Garbage nametable reads
        ppu_fake_memory_access(ppu);
        ppu_fake_memory_access(ppu);

        // TODO:
        // Read tile bitmap low
        // Read tile bitmap high (at low addr + 8)

        // If there are less than 8 sprites on the next scanline, then dummy
        // fetches to tile $FF occur for the left-over sprites, because of the
        // dummy sprite data in the secondary OAM (see sprite evaluation). This
        // data is then discarded, and the sprites are loaded with a transparent
        // bitmap instead.
    }

    // Get first 2 tiles for the *next* scanline
    for (int i = 0; i < 2; ++i) {
        // TODO:
        // Read nametable byte
        // Read Attribute table byte
        // Read tile bitmap low
        // Read tile bitmap high (at low addr + 8)
    }

    // Two bytes are fetched, but the purpose for this is unknown
    ppu_fake_memory_access(ppu);
    ppu_fake_memory_access(ppu);
}

void ppu_idle_scanline(PPU_t* ppu) {
    for (int i = 0; i < CYCLES_PER_SCANLINE; ++i)
        ppu_tick(ppu);
}

void ppu_vblank_scanline(PPU_t* ppu) {
    ppu_tick(ppu); // Cycle 0
    ppu_tick(ppu); // Cycle 1

    ppu->reg_PPUSTATUS = set_bit(ppu->reg_PPUSTATUS, stat_VBLANK, true);
    if (get_bit(ppu->reg_PPUCTRL, ctrl_NMI))
        ppu->cpu->sig_NMI = false;

    // Set i = 2 because we've already performed two cycles for this scanline
    for (int i = 2; i < CYCLES_PER_SCANLINE; ++i)
        ppu_tick(ppu);
}

uint8_t ppu_get_pallette(PPU_t* ppu, bool sprite, uint8_t num, uint8_t value) {
    uint8_t index = (sprite ? 0x10 : 0) | (num << 2) | value;
    return ppu->pallette_indices[index & 0b00011111];
}

// Helper functions
bool ppu_rendering_enabled(PPU_t* ppu) {
    // Rendering is considered disabled if both the sprite and background layers
    // are disabled
    return (ppu->reg_PPUMASK & RENDERING_MASK) > 0;
}

uint16_t ppu_base_nametable(PPU_t* ppu) {
    return 0x2000 + (ppu->reg_PPUCTRL & ctrl_BASENTABLE) * (NAMETABLE_SIZE);
}

uint16_t ppu_base_patterntable(PPU_t* ppu) {
    return get_bit(ppu->reg_PPUCTRL, ctrl_BGPTABLE) ? 0x0000 : 0x1000;
}

uint8_t ppu_vram_inc(PPU_t* ppu) {
    return get_bit(ppu->reg_PPUCTRL, ctrl_VRAMINC) ? 32 : 1;
}

const uint8_t* ppu_rgb_from_pallette(PPU_t* ppu, uint8_t i) {
    return REF_PALLETTE_MAP[i & 0b00111111];
}

// Memory functions
void ppu_write_oam_from_reg(PPU_t* ppu) {
    ppu->oam[ppu->reg_OAMADDR++] = ppu->reg_OAMDATA;
}

uint8_t* ppu_read_oam_from_reg(PPU_t* ppu) {
    if (get_bit(ppu->reg_PPUSTATUS, stat_VBLANK))
        return &ppu->oam[ppu->reg_OAMDATA];
    else
        return &ppu->oam[ppu->reg_OAMDATA++];
}

void ppu_fake_memory_access(PPU_t* ppu) {
    ppu_tick(ppu);
    ppu_tick(ppu);
}

uint8_t* ppu_memory_map_read(PPU_t* ppu, uint16_t address) {
    ppu_tick(ppu);
    ppu_tick(ppu);

    // The current 8KiB page of CHR ROM is mapped onto the $0000 - $2000 range
    // of the PPU memory map.
    if (address < 0x2000)
        return &ppu->cartridge->chr_data[address];
    // The PPU nametables are mapped onto $2000 - $3000.
    else if (address >= 0x2000 && address < 0x3000)
        return ppu_nametable_read(ppu, address - 0x2000);
    // A mirror of $2000 - $2EFF exists in the range $3000 - $3EFF.
    else if (address >= 0x3000 && address < 0x3EFF)
        return ppu_nametable_read(ppu, address - 0x3000);
    // The rest of memory is filled with repeating mirrors of the pallete
    // indices.
    else if (address >= 0x3F00 && address < 0x4000) {
        uint8_t i = (address - 0x3F00) % PALLETTE_IND_SIZE;
        switch (i) {
            // Addresses $3F10, $3F14, $3F18, and $3F1C map to $3F0X
            case 0x10:
            case 0x14:
            case 0x18:
            case 0x1C:
                return &ppu->pallette_indices[i - 0x10];
            default:
                return &ppu->pallette_indices[i];
        }
    // Addresses above $3FFF are mirrors of $0000 - $3FFF
    } else
        return ppu_memory_map_read(ppu, address - 0x4000);
}

uint8_t* ppu_memory_map_read_inc(PPU_t* ppu, uint16_t address) {
    ppu->reg_PPUADDR += ppu_vram_inc(ppu);
    return ppu_memory_map_read(ppu, address);
}

void ppu_memory_map_write(PPU_t* ppu, uint16_t address, uint8_t value) {
    ppu_tick(ppu);
    ppu_tick(ppu);

    if (address >= 0x2000)
        *ppu_memory_map_read(ppu, address) = value;
}

void ppu_memory_map_write_inc(PPU_t* ppu, uint16_t address, uint8_t value) {
    ppu->reg_PPUADDR += ppu_vram_inc(ppu);
    ppu_memory_map_write(ppu, address, value);
}

uint8_t* ppu_nametable_read(PPU_t* ppu, uint16_t address) {
    uint8_t  relative_addr = address % (NAMETABLE_SIZE);
    uint8_t* table1_result = &ppu->memory[relative_addr];
    uint8_t* table2_result = &ppu->memory[(NAMETABLE_SIZE) + relative_addr];

    // Switch over the nametable index
    switch (address / (NAMETABLE_SIZE)) {
        case 0: return table1_result;
        case 1: return ppu->mirroring ? table1_result : table2_result;
        case 2: return ppu->mirroring ? table2_result : table1_result;
        case 3: return table2_result;
        default: return NULL;
    }
}
