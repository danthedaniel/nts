#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "rom.h"
#include "console.h"
#include "util.h"

ROM_t* rom_from_file(char* path) {
    ROM_t* rom = NULL;
    uint8_t *buffer = NULL;
    uint32_t file_size = 0;
    FILE* rom_file = fopen(path, "rb");

    if (rom_file == NULL) {
        fprintf(stderr, "Error: Could not open file %s\n", path);
        goto cleanup_fhandler;
    }

    fseek(rom_file, 0, SEEK_END);
    file_size = ftell(rom_file);

    // Make sure the file is big enough for a header
    if (file_size < (HEADER_SIZE)) {
        fprintf(stderr, "Error: File too small, only %d bytes\n", file_size);
        goto cleanup_fhandler;
    }

    rewind(rom_file);
    buffer = (uint8_t*) malloc(file_size * sizeof(char));
    fread(buffer, file_size, 1, rom_file);

    // Check for magic bytes
    if (!(buffer[0] == 'N') ||
        !(buffer[1] == 'E') ||
        !(buffer[2] == 'S') ||
        !(buffer[3] == 0x1A)) {
        fprintf(stderr, "Error: No magic bytes\n");
        goto cleanup_filebuffer;
    }

    rom = (ROM_t*) malloc(sizeof(ROM_t));

    rom->prg_page = 0;
    rom->prg_page_count = buffer[4];
    rom->chr_page_count = buffer[5];
    rom->ram_page_count = buffer[8];

    rom->flags6 = buffer[6];
    rom->flags7 = buffer[7];
    rom->flags9 = buffer[9];

    if (!rom_file_valid(rom, file_size)) {
        fprintf(stderr, "Error: File is not valid\n");
        free(rom);
        return NULL;
    }

    rom_load_pages(rom, buffer);

cleanup_filebuffer:
    free(buffer);
cleanup_fhandler:
    fclose(rom_file);

    return rom;
}

void rom_free(ROM_t* rom) {
    free(rom->prg_data);
    free(rom->chr_data);
    free(rom->ram_data);
    free(rom->trainer_data);
    free(rom);
}

bool rom_file_valid(ROM_t* rom, uint32_t buffer_len) {
    if (rom->prg_page_count == 0) {
        return false;
    }

    uint32_t rom_computed_size = (HEADER_SIZE) +
        (rom->prg_page_count * (PRG_PAGE_SIZE)) +
        (rom->chr_page_count * (CHR_PAGE_SIZE)) +
        (get_bit(rom->flags6, TRAINER) ? (TRAINER_SIZE) : 0);

    return rom_computed_size <= buffer_len;
}

void rom_load_pages(ROM_t* rom, uint8_t* buffer) {
    buffer += HEADER_SIZE; // Skip past headers

    // Copy trainer data if present
    if (get_bit(rom->flags6, TRAINER)) {
        rom->trainer_data = (uint8_t*) malloc(TRAINER_SIZE);
        memcpy(rom->trainer_data, buffer, TRAINER_SIZE);

        buffer += TRAINER_SIZE;
    } else {
        rom->trainer_data = NULL;
    }

    // Copy PRG data
    uint32_t prg_data_size = rom->prg_page_count * (PRG_PAGE_SIZE);
    rom->prg_data = (uint8_t*) malloc(prg_data_size);
    memcpy(rom->prg_data, buffer, prg_data_size);
    buffer += prg_data_size;

    // Copy CHR data
    uint32_t chr_data_size = rom->prg_page_count * (CHR_PAGE_SIZE);
    rom->chr_data = (uint8_t*) malloc(chr_data_size);
    memcpy(rom->chr_data, buffer, chr_data_size);

    // Also initialize catridge RAM
    if (rom->ram_page_count > 0)
        rom->ram_data = (uint8_t*) malloc(rom->ram_page_count * (RAM_PAGE_SIZE));
    else
        rom->ram_data = NULL;
}

uint8_t rom_mapper(ROM_t* rom) {
    return (rom->flags7 & MAPPER_UPPER) | ((rom->flags6 & MAPPER_LOWER) >> 4);
}

bool rom_nes2(ROM_t* rom) {
    return (rom->flags7 & NES2_FORMAT) == 0b00001000;
}

void rom_print_details(ROM_t* rom) {
    printf("rom\n");
    printf("\t->prg_page_count %d\n", rom->prg_page_count);
    printf("\t->chr_page_count %d\n", rom->chr_page_count);
    printf("\t->ram_page_count %d\n", rom->ram_page_count);
    printf("\t->flags6 %02x\n", rom->flags6);
    printf("\t->flags7 %02x\n", rom->flags7);
    printf("\t->flags9 %02x\n", rom->flags9);
}

uint8_t* rom_map_read(ROM_t* rom, uint16_t address) {
    uint8_t mapper = rom_mapper(rom);

    switch (mapper) {
        case 0:
            if (address >= 0x6000 && address < 0x8000) {
                if (rom->ram_page_count > 0)
                    return &rom->ram_data[address - 0x6000];
                else
                    return &ZERO;
            }

            if (address >= 0x8000 && address < 0xC000)
                return &rom->prg_data[address - 0x8000];

            if (address >= 0xC000) {
                if (rom->prg_page_count == 1)
                    return &rom->prg_data[address - 0xC000]; // Mirror first page
                else if (rom->prg_page_count == 2)
                    return &rom->prg_data[address - 0xC000 + (PRG_PAGE_SIZE)];
                else
                    return &ZERO;
            }
        default:
            fprintf(stderr, "Error: Unsupported mapper %d\n", mapper);
            return &ZERO;
    }
}
