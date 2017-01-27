#ifndef CPU_H__
#define CPU_H__

#include <stdint.h>
#include "console.h"
#include "rom.h"
#include "ppu.h"

#define CPU_CLOCK (MASTER_CLOCK) / 12.0
#define STACK_OFFSET 0x0100

// Addressing modes
typedef enum {
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
} AddrMode;

enum StatusBits {
    stat_NEGATIVE = 7,
    stat_OVERFLOW = 6,
    stat_B5       = 5,
    stat_B4       = 4,
    stat_DECIMAL  = 3,
    stat_INT      = 2,
    stat_ZERO     = 1,
    stat_CARRY    = 0
};

CPU_t* cpu_init(ROM_t* cartridge);
void cpu_free();

// Returns cycle count. A return value of 0 represents a crash
uint8_t cpu_cycle(CPU_t* cpu); // Returns cycle count
void cpu_start(CPU_t* cpu);

// Memory functions
uint8_t cpu_map_read(CPU_t* cpu, uint16_t address);
void cpu_map_write(CPU_t* cpu, uint16_t address, uint8_t value);
uint16_t cpu_address_from_mode(CPU_t* cpu, AddrMode mode, bool* page_cross);
bool cpu_page_crossed(uint16_t address_before, uint16_t address_after);

// Stack functions
void cpu_stack_push(CPU_t* cpu, uint8_t value);
uint8_t cpu_stack_pull(CPU_t* cpu);

// Instructions
// Each instruction returns any additional clock cycles that the operation
// would take to perform.
uint8_t op_adc(CPU_t* cpu, AddrMode mode);
uint8_t op_and(CPU_t* cpu, AddrMode mode);
uint8_t op_asl(CPU_t* cpu, AddrMode mode);
uint8_t op_bcc(CPU_t* cpu, AddrMode mode);
uint8_t op_bcs(CPU_t* cpu, AddrMode mode);
uint8_t op_beq(CPU_t* cpu, AddrMode mode);
uint8_t op_bit(CPU_t* cpu, AddrMode mode);
uint8_t op_bmi(CPU_t* cpu, AddrMode mode);
uint8_t op_bne(CPU_t* cpu, AddrMode mode);
uint8_t op_bpl(CPU_t* cpu, AddrMode mode);
uint8_t op_brk(CPU_t* cpu, AddrMode mode);
uint8_t op_bvc(CPU_t* cpu, AddrMode mode);
uint8_t op_bvs(CPU_t* cpu, AddrMode mode);
uint8_t op_clc(CPU_t* cpu, AddrMode mode);
uint8_t op_cld(CPU_t* cpu, AddrMode mode);
uint8_t op_cli(CPU_t* cpu, AddrMode mode);
uint8_t op_clv(CPU_t* cpu, AddrMode mode);
uint8_t op_cmp(CPU_t* cpu, AddrMode mode);
uint8_t op_cmx(CPU_t* cpu, AddrMode mode);
uint8_t op_cmy(CPU_t* cpu, AddrMode mode);
uint8_t op_dec(CPU_t* cpu, AddrMode mode);
uint8_t op_dex(CPU_t* cpu, AddrMode mode);
uint8_t op_dey(CPU_t* cpu, AddrMode mode);
uint8_t op_eor(CPU_t* cpu, AddrMode mode);
uint8_t op_inc(CPU_t* cpu, AddrMode mode);
uint8_t op_inx(CPU_t* cpu, AddrMode mode);
uint8_t op_iny(CPU_t* cpu, AddrMode mode);
uint8_t op_jmp(CPU_t* cpu, AddrMode mode);
uint8_t op_jsr(CPU_t* cpu, AddrMode mode);
uint8_t op_lda(CPU_t* cpu, AddrMode mode);
uint8_t op_ldx(CPU_t* cpu, AddrMode mode);
uint8_t op_ldy(CPU_t* cpu, AddrMode mode);
uint8_t op_lsr(CPU_t* cpu, AddrMode mode);
uint8_t op_ora(CPU_t* cpu, AddrMode mode);
uint8_t op_pha(CPU_t* cpu, AddrMode mode);
uint8_t op_php(CPU_t* cpu, AddrMode mode);
uint8_t op_pla(CPU_t* cpu, AddrMode mode);
uint8_t op_plp(CPU_t* cpu, AddrMode mode);
uint8_t op_rol(CPU_t* cpu, AddrMode mode);
uint8_t op_ror(CPU_t* cpu, AddrMode mode);
uint8_t op_rti(CPU_t* cpu, AddrMode mode);
uint8_t op_rts(CPU_t* cpu, AddrMode mode);
uint8_t op_sbc(CPU_t* cpu, AddrMode mode);
uint8_t op_sec(CPU_t* cpu, AddrMode mode);
uint8_t op_sed(CPU_t* cpu, AddrMode mode);
uint8_t op_sei(CPU_t* cpu, AddrMode mode);
uint8_t op_sta(CPU_t* cpu, AddrMode mode);
uint8_t op_stx(CPU_t* cpu, AddrMode mode);
uint8_t op_sty(CPU_t* cpu, AddrMode mode);
uint8_t op_tax(CPU_t* cpu, AddrMode mode);
uint8_t op_tay(CPU_t* cpu, AddrMode mode);
uint8_t op_tsx(CPU_t* cpu, AddrMode mode);
uint8_t op_txa(CPU_t* cpu, AddrMode mode);
uint8_t op_txs(CPU_t* cpu, AddrMode mode);
uint8_t op_tya(CPU_t* cpu, AddrMode mode);

#endif
