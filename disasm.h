#ifndef __DISASM_H__
#define __DISASM_H__

#include <stdint.h>
#include <stdbool.h>

#define HEADER_SIZE   1<<4
#define TRAINER_SIZE  1<<9
#define PRG_PAGE_SIZE 1<<14
#define CHR_PAGE_SIZE 1<<13
#define RAM_PAGE_SIZE 1<<13

typedef struct {
    bool mirroring;
    bool ram_battery;
    bool trainer;
    bool ignore_mirror;
    uint8_t mapper;
} FLAGS_t;

typedef struct {
    uint8_t prg_page_count; // 16KiB units
    uint8_t* prg_data;
    uint8_t chr_page_count; // 8KiB units
    uint8_t* chr_data;
    uint8_t ram_page_count; // 8KiB units
    uint8_t* ram_data;
    uint8_t* trainer_data;
    FLAGS_t* flags;
} ROM_t;

ROM_t* rom_from_file(char* path);
void rom_load_pages(ROM_t* rom, uint8_t* buffer, uint32_t buffer_len);
void rom_free(ROM_t* rom);

enum Flag6Mask {
    MIRRORING      = 1 << 0,
    RAM_BATTERY    = 1 << 1,
    TRAINER        = 1 << 2,
    IGNORE_MIRROR  = 1 << 3,
    MAPPER         = 0xF0
};

#endif
