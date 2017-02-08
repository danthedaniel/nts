#ifndef EMULATOR_H__
#define EMULATOR_H__

#include <pthread.h>
#include "rom.h"

enum ThreadNames {
  CPU_THREAD,
  PPU_THREAD,
  APU_THREAD,
  NUM_THREADS
};

pthread_t tids[NUM_THREADS];

void system_bootstrap(ROM_t* cartridge);

void* cpu_thread(void* arg);
void* ppu_thread(void* arg);
void* apu_thread(void* arg);

#endif
