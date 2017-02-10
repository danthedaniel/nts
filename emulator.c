#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>
#include "emulator.h"
#include "console.h"
#include "cpu.h"
#include "rom.h"

void system_bootstrap(ROM_t* cartridge) {
    if (pthread_mutex_init(&clock_lock, NULL) != 0) {
        fprintf(stderr, "Unable to create mutex lock\n");
        return;
    }

    CPU_t* cpu = cpu_init(cartridge);

    int cpuErr = pthread_create(&(tids[CPU_THREAD]), NULL, &cpu_thread, (void*) cpu);
    int ppuErr = pthread_create(&(tids[PPU_THREAD]), NULL, &cpu_thread, (void*) cpu->ppu);

    if (cpuErr != 0) {
        fprintf(stderr, "Unable to start CPU thread\n");
        pthread_cancel(tids[PPU_THREAD]);
    }

    if (ppuErr != 0) {
        fprintf(stderr, "Unable to start PPU thread\n");
        cpu->powered_on = false;
        pthread_cancel(tids[CPU_THREAD]);
    }

    pthread_join(tids[CPU_THREAD], NULL);
    pthread_join(tids[PPU_THREAD], NULL);
    pthread_mutex_destroy(&clock_lock);
}

void* cpu_thread(void* arg) {
    CPU_t* cpu = (CPU_t*) arg;

    cpu_start(cpu);
    cpu_free(cpu);

    return NULL;
}

void* ppu_thread(void* arg) {
    PPU_t* ppu = (PPU_t*) arg;

    ppu_start(ppu);
    ppu_free(ppu);

    return NULL;
}
