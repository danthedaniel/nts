#include <stdio.h>
#include "rom.h"
#include "cpu.h"

int main(int argc, char* argv[]) {
    if (argc > 1) {
        printf("Reading in %s\n", argv[1]);
        ROM_t* rom = rom_from_file(argv[1]);

        if (rom != NULL) {
            rom_print_details(rom);

            CPU_t* cpu = cpu_init(rom);
            cpu_start(cpu);

            cpu_free(cpu);
            rom_free(rom);
        } else {
            return 1; // Could not open file
        }
    }

    return 0;
}
