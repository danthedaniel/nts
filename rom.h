#ifndef ROM_H__
#define ROM_H__

#include <stdint.h>
#include <stdbool.h>

#define HEADER_SIZE   1<<4  // 16B
#define TRAINER_SIZE  1<<9  // 512B
#define PRG_PAGE_SIZE 1<<14 // 16KiB
#define CHR_PAGE_SIZE 1<<13 // 8KiB
#define RAM_PAGE_SIZE 1<<13 // 8KiB

typedef struct {
    uint8_t  prg_page;
    uint8_t  prg_page_count;
    uint8_t* prg_data;
    uint8_t  chr_page_count;
    uint8_t* chr_data;
    uint8_t  ram_page_count;
    uint8_t* ram_data;
    uint8_t* trainer_data;

    uint8_t flags6;
    uint8_t flags7;
    uint8_t flags9;
} ROM_t;

ROM_t* rom_from_file(char* path);
bool rom_file_valid(ROM_t* rom, uint32_t buffer_len);
void rom_load_pages(ROM_t* rom, uint8_t* buffer);
void rom_free(ROM_t* rom);

uint8_t rom_map_read(ROM_t* rom, uint16_t address);

uint8_t rom_mapper(ROM_t* rom);

void rom_print_details(ROM_t* rom);

enum Flag6Masks {
    MIRRORING      = 0,
    RAM_BATTERY    = 1,
    TRAINER        = 2,
    IGNORE_MIRROR  = 3,
    MAPPER_LOWER   = 0b11110000
};

enum Flag7Masks {
    VS_UNISYSTEM  = 0,
    PLAYCHOICE    = 1,
    NES2_FORMAT   = 0b00001100,
    MAPPER_UPPER  = 0b11110000
};

enum Flag9Masks {
    TV_SYSTEM     = 0
};

#endif
