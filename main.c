#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include "rom.h"
#include "cpu.h"

void INThandler(int sig);

int main(int argc, char* argv[]) {
    signal(SIGINT, INThandler);

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


void INThandler(int sig) {
    signal(sig, SIG_IGN);
    printf("Recieved ^C, shutting down\n");
    exit(0);
}
