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
void cpu_start(CPU_t* cpu);
uint8_t cpu_map_read(CPU_t* cpu, uint16_t address);
void cpu_map_write(CPU_t* cpu, uint16_t address, uint8_t value);

// Addressing modes
enum AddressingModes {
    IMPLICIT,
    ACCUMULATOR,
    IMMEDIATE,
    ZERO_PAGE,
    ZERO_PAGE_X,
    ZERO_PAGE_Y,
    RELATIVE,
    ABSOLUTE,
    ABSOLUTE_X,
    ABSOLUTE_Y,
    INDIRECT,
    INDX_IND,
    IND_INDX
};

// Instructions
// Each instruction returns any additional clock cycles that the operation
// would take to perform.
uint8_t op_adc(CPU_t* cpu, uint8_t mode);
uint8_t op_and(CPU_t* cpu, uint8_t mode);
uint8_t op_asl(CPU_t* cpu, uint8_t mode);
uint8_t op_bcc(CPU_t* cpu, uint8_t mode);
uint8_t op_bcs(CPU_t* cpu, uint8_t mode);

uint16_t cpu_address_from_mode(CPU_t* cpu, uint8_t mode);

enum StatusBits {
    stat_NEGATIVE = 7,
    stat_OVERFLOW = 6,
    stat_DECIMAL  = 3,
    stat_INT      = 2,
    stat_ZERO     = 1,
    stat_CARRY    = 0
};

#endif
