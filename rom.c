#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "rom.h"

ROM_t* rom_from_file(char* path) {
    ROM_t* rom = NULL;
    uint8_t *buffer;
    uint32_t file_size;

    FILE* rom_file = fopen(path, "rb");

    if (rom_file != NULL) {
        fseek(rom_file, 0, SEEK_END);
        file_size = ftell(rom_file);

        // Make sure the file is at least 16B + 16KiB in size,
        // which is the minimum ROM size
        if (file_size >= (HEADER_SIZE) + (PRG_PAGE_SIZE)) {
            rewind(rom_file);
            buffer = (uint8_t*) malloc(file_size * sizeof(char));
            fread(buffer, file_size, 1, rom_file);

            // Check for magic number
            if ((buffer[0] == 0x4E) && // N
                (buffer[1] == 0x45) && // E
                (buffer[2] == 0x53) && // S
                (buffer[3] == 0x1A)) { // \r
                // malloc space for ROM struct
                rom = (ROM_t*) malloc(sizeof(ROM_t));
                FLAGS_t* flags = (FLAGS_t*) malloc(sizeof(FLAGS_t));
                rom->flags = flags;

                rom->prg_page_count = buffer[4];
                rom->chr_page_count = buffer[5];
                rom->ram_page_count = buffer[8];

                rom->ram_data = NULL;

                rom->flags->mirroring     = (buffer[6] & MIRRORING);
                rom->flags->ram_battery   = (buffer[6] & RAM_BATTERY) >> 1;
                rom->flags->trainer       = (buffer[6] & TRAINER) >> 2;
                rom->flags->ignore_mirror = (buffer[6] & IGNORE_MIRROR) >> 3;
                rom->flags->mapper        = (buffer[6] & MAPPER) >> 4;

                rom_load_pages(rom, buffer, file_size);
            } else {
                fprintf(stderr, "Error: No magic bytes\n");
            }

            free(buffer);
        } else {
            fprintf(stderr, "Error: File too small, %d bytes\n", file_size);
        }
    } else {
        fprintf(stderr, "Error: Could not open file %s\n", path);
    }

    fclose(rom_file);

    return rom;
}

void rom_load_pages(ROM_t* rom, uint8_t* buffer, uint32_t buffer_len) {
    uint32_t buffer_loc = HEADER_SIZE; // Skip past headers

    // Copy trainer data if present
    if (rom->flags->trainer) {
        rom->trainer_data = (uint8_t*) malloc(TRAINER_SIZE);
        memcpy(rom->trainer_data, &buffer[buffer_loc], TRAINER_SIZE);

        buffer_loc += TRAINER_SIZE;
    } else {
        rom->trainer_data = NULL;
    }

    // Copy PRG data
    uint32_t prg_data_size = rom->prg_page_count * (PRG_PAGE_SIZE);
    rom->prg_data = (uint8_t*) malloc(prg_data_size);
    memcpy(rom->prg_data, &buffer[buffer_loc], prg_data_size);
    buffer_loc += prg_data_size;

    // Copy CHR data
    uint32_t chr_data_size = rom->prg_page_count * (CHR_PAGE_SIZE);
    rom->chr_data = (uint8_t*) malloc(chr_data_size);
    memcpy(rom->chr_data, &buffer[buffer_loc], chr_data_size);
    buffer_loc += chr_data_size;
}

void rom_print_details(ROM_t* rom) {
    printf("rom\n");
    printf("\t->prg_page_count %d\n", rom->prg_page_count);
    printf("\t->chr_page_count %d\n", rom->chr_page_count);
    printf("\t->ram_page_count %d\n", rom->ram_page_count);
    printf("\t->flags\n");
    printf("\t\t->mapper        %d\n", rom->flags->mapper);
    printf("\t\t->mirroring     %d\n", rom->flags->mirroring);
    printf("\t\t->ram_battery   %d\n", rom->flags->ram_battery);
    printf("\t\t->trainer       %d\n", rom->flags->trainer);
    printf("\t\t->ignore_mirror %d\n", rom->flags->ignore_mirror);
}

void rom_free(ROM_t* rom) {
    free(rom->flags);
    free(rom->prg_data);
    free(rom->chr_data);
    free(rom->ram_data);
    free(rom->trainer_data);
    free(rom);
}
