#ifndef CPU_H__
#define CPU_H__

#include <stdint.h>
#include <stdbool.h>
#include "console.h"
#include "rom.h"
#include "ppu.h"

#define DEBUG

#define CPU_CLOCK (MASTER_CLOCK) / 12.0
#define STACK_OFFSET 0x0100

#define BRK_VECTOR 0xFFFE
#define RST_VECTOR 0xFFFC
#define NMI_VECTOR 0xFFFA

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

enum CPUStatusBits {
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
void cpu_free(CPU_t* cpu);

void cpu_perform_op(CPU_t* cpu);
void cpu_start(CPU_t* cpu);
void cpu_tick(CPU_t* cpu);
uint16_t cpu_get_vector(CPU_t* cpu, uint16_t vec_start);

// Signal handlers
void cpu_irq(CPU_t* cpu);
void cpu_nmi(CPU_t* cpu);

// Memory functions
uint8_t* cpu_map_read(CPU_t* cpu, uint16_t address);
void cpu_write_back(CPU_t* cpu, uint8_t* address, uint8_t value);
void cpu_map_write(CPU_t* cpu, uint16_t address, uint8_t value);
uint16_t cpu_address_from_mode(CPU_t* cpu, AddrMode mode, bool page_cross_cost);
uint8_t* cpu_get_op_target(CPU_t* cpu, AddrMode mode, bool page_cross_cost);
void cpu_oam_transfer(CPU_t* cpu);

// Stack functions
void cpu_stack_push(CPU_t* cpu, uint8_t value);
uint8_t cpu_stack_pull(CPU_t* cpu);

// Debug functions
void cpu_print_regs(CPU_t* cpu);

// Instructions
// Each instruction returns any additional clock cycles that the operation
// would take to perform.
void op_adc(CPU_t* cpu, AddrMode mode);
void op_and(CPU_t* cpu, AddrMode mode);
void op_asl(CPU_t* cpu, AddrMode mode);
void op_bcc(CPU_t* cpu, AddrMode mode);
void op_bcs(CPU_t* cpu, AddrMode mode);
void op_beq(CPU_t* cpu, AddrMode mode);
void op_bit(CPU_t* cpu, AddrMode mode);
void op_bmi(CPU_t* cpu, AddrMode mode);
void op_bne(CPU_t* cpu, AddrMode mode);
void op_bpl(CPU_t* cpu, AddrMode mode);
void op_brk(CPU_t* cpu, AddrMode mode);
void op_bvc(CPU_t* cpu, AddrMode mode);
void op_bvs(CPU_t* cpu, AddrMode mode);
void op_clc(CPU_t* cpu, AddrMode mode);
void op_cld(CPU_t* cpu, AddrMode mode);
void op_cli(CPU_t* cpu, AddrMode mode);
void op_clv(CPU_t* cpu, AddrMode mode);
void op_cmp(CPU_t* cpu, AddrMode mode);
void op_cmx(CPU_t* cpu, AddrMode mode);
void op_cmy(CPU_t* cpu, AddrMode mode);
void op_dec(CPU_t* cpu, AddrMode mode);
void op_dex(CPU_t* cpu, AddrMode mode);
void op_dey(CPU_t* cpu, AddrMode mode);
void op_eor(CPU_t* cpu, AddrMode mode);
void op_inc(CPU_t* cpu, AddrMode mode);
void op_inx(CPU_t* cpu, AddrMode mode);
void op_iny(CPU_t* cpu, AddrMode mode);
void op_jmp(CPU_t* cpu, AddrMode mode);
void op_jsr(CPU_t* cpu, AddrMode mode);
void op_lda(CPU_t* cpu, AddrMode mode);
void op_ldx(CPU_t* cpu, AddrMode mode);
void op_ldy(CPU_t* cpu, AddrMode mode);
void op_lsr(CPU_t* cpu, AddrMode mode);
void op_nop(CPU_t* cpu, AddrMode mode);
void op_ora(CPU_t* cpu, AddrMode mode);
void op_pha(CPU_t* cpu, AddrMode mode);
void op_php(CPU_t* cpu, AddrMode mode);
void op_pla(CPU_t* cpu, AddrMode mode);
void op_plp(CPU_t* cpu, AddrMode mode);
void op_rol(CPU_t* cpu, AddrMode mode);
void op_ror(CPU_t* cpu, AddrMode mode);
void op_rti(CPU_t* cpu, AddrMode mode);
void op_rts(CPU_t* cpu, AddrMode mode);
void op_sbc(CPU_t* cpu, AddrMode mode);
void op_sec(CPU_t* cpu, AddrMode mode);
void op_sed(CPU_t* cpu, AddrMode mode);
void op_sei(CPU_t* cpu, AddrMode mode);
void op_sta(CPU_t* cpu, AddrMode mode);
void op_stx(CPU_t* cpu, AddrMode mode);
void op_sty(CPU_t* cpu, AddrMode mode);
void op_tax(CPU_t* cpu, AddrMode mode);
void op_tay(CPU_t* cpu, AddrMode mode);
void op_tsx(CPU_t* cpu, AddrMode mode);
void op_txa(CPU_t* cpu, AddrMode mode);
void op_txs(CPU_t* cpu, AddrMode mode);
void op_tya(CPU_t* cpu, AddrMode mode);

#endif
