#include <stdio.h>
#include "disasm.h"

int main(int argc, char* argv[]) {
    if (argc > 1) {
        printf("Reading in %s", argv[1]);
        ROM_t* rom = rom_from_file(argv[1]);
        rom_free(rom);
    }

    return 0;
}
