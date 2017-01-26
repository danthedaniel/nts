#ifndef CPU_H__
#define CPU_H__

#include <stdint.h>
#include "console.h"
#include "rom.h"
#include "ppu.h"

#define CPU_CLOCK (MASTER_CLOCK) / 12.0

CPU_t* cpu_init(ROM_t* cartridge);
void cpu_free();

uint8_t cpu_cycle(CPU_t* cpu); // Returns cycle count
uint8_t cpu_memory_map_read(CPU_t* cpu, uint16_t address);
void cpu_memory_map_write(CPU_t* cpu, uint16_t address, uint8_t value);

#endif
