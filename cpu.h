#ifndef CPU_H__
#define CPU_H__

#include <stdint.h>
#include "console.h"
#include "rom.h"
#include "ppu.h"

CPU_t* cpu_init();
void cpu_free();

uint8_t cpu_memory_map_read(CPU_t* cpu, uint16_t address);
void cpu_memory_map_write(CPU_t* cpu, uint16_t address, uint8_t value);

void cpu_clock(CPU_t* cpu);

#endif
