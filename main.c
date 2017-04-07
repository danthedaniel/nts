#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include "emulator.h"
#include "rom.h"

void INThandler(int sig);
void print_help();

int main(int argc, char* argv[]) {
    signal(SIGINT, INThandler);

    if (argc <= 1) {
        print_help();
        return 1;
    }

    printf("Reading in %s\n", argv[1]);
    ROM_t* rom = rom_from_file(argv[1]);

    if (rom == NULL) {
        fprintf(stderr, "Could not read ROM file\n");
        return 1;
    }

    system_bootstrap(rom);
    rom_free(rom);

    return 0;
}

void INThandler(int sig) {
    signal(sig, SIG_IGN);
    printf("Recieved ^C, shutting down\n");
    exit(0);
}

void print_help() {
    fprintf(stderr, "Syntax: nts rompath\n");
}
