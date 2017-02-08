#include <stdio.h>
#include <pthread.h>
#include "emulator.h"
#include "console.h"
#include "cpu.h"
#include "rom.h"

void system_bootstrap(ROM_t* cartridge) {
    if (pthread_mutex_init(&clock_lock, NULL) != 0) {
        fprintf(stderr, "Unable to create mutex lock\n");
        return;
    }

    int err = pthread_create(&(tids[CPU_THREAD]), NULL, &cpu_thread, (void*) cartridge);

    if (err != 0)
        fprintf(stderr, "Unable to start CPU thread\n");

    pthread_join(tids[CPU_THREAD], NULL);
    pthread_join(tids[PPU_THREAD], NULL);
    pthread_mutex_destroy(&clock_lock);
}

void* cpu_thread(void* arg) {
    ROM_t* cartridge = (ROM_t*) arg;
    CPU_t* cpu = cpu_init(cartridge);

    // Give the lock to the CPU thread
    pthread_mutex_lock(&clock_lock);
    // Start the PPU thread
    int err = pthread_create(&(tids[PPU_THREAD]), NULL, &ppu_thread, (void*) cpu->ppu);

    if (err != 0) {
        fprintf(stderr, "Unable to start PPU thread\n");
        pthread_mutex_unlock(&clock_lock);
        return NULL;
    }

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
