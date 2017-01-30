#include <stdint.h>
// #include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "cpu.h"
#include "ppu.h"
#include "apu.h"
#include "rom.h"
#include "util.h"

CPU_t* cpu_init(ROM_t* cartridge) {
    CPU_t* cpu = (CPU_t*) malloc(sizeof(CPU_t));

    // Zero out system memory
    memset(cpu->memory, 0, CPU_MEMORY_SIZE);

    // Set up registers
    cpu->reg_A = 0;
    cpu->reg_X = 0;
    cpu->reg_Y = 0;
    cpu->reg_P = 0x34;
    cpu->reg_S = 0xFD;

    cpu->cycle = 0;

    // Connect hardware
    cpu->ppu = ppu_init();
    cpu->apu = apu_init();
    cpu->cartridge = cartridge;

    return cpu;
}

void cpu_free(CPU_t* cpu) {
    free(cpu->ppu);
    free(cpu->apu);
    free(cpu);
}

uint16_t cpu_get_vector(CPU_t* cpu, uint16_t vec_start) {
    uint8_t lower_addr = *cpu_map_read(cpu, vec_start);
    uint8_t upper_addr = *cpu_map_read(cpu, vec_start + 1);
    return (((uint16_t) upper_addr) << 8) | lower_addr;
}

void cpu_start(CPU_t* cpu) {
    cpu->powered_on = true;
    cpu->reg_PC = cpu_get_vector(cpu, RST_VECTOR);

    while (cpu->powered_on) {
#ifdef DEBUG
        printf("Cycle %08x:\n", (uint32_t) cpu->cycle);
#endif

        cpu_perform_op(cpu);

#ifdef DEBUG
        cpu_print_regs(cpu);
        printf("Waiting...\n");
        getchar();
#endif
    }

    printf("CPU shutting down\n");
}

void cpu_tick(CPU_t* cpu) {
    cpu->cycle++;
    ppu_tick(cpu->ppu);
    ppu_tick(cpu->ppu);
    ppu_tick(cpu->ppu);
}

void cpu_perform_op(CPU_t* cpu) {
    uint16_t orig_pc = cpu->reg_PC;
    uint8_t opcode = *cpu_map_read(cpu, cpu->reg_PC++);

#ifdef DEBUG
    printf("$%04x EXEC %02x\n", orig_pc, opcode);
#endif

    switch(opcode) {
        // ADC
        case 0x69: op_adc(cpu, IMMEDIATE);   break;
        case 0x65: op_adc(cpu, ZERO_PAGE);   break;
        case 0x75: op_adc(cpu, ZERO_PAGE_X); break;
        case 0x6D: op_adc(cpu, ABSOLUTE);    break;
        case 0x7D: op_adc(cpu, ABSOLUTE_X);  break;
        case 0x79: op_adc(cpu, ABSOLUTE_Y);  break;
        case 0x61: op_adc(cpu, INDX_IND);    break;
        case 0x71: op_adc(cpu, IND_INDX);    break;
        // AND
        case 0x29: op_and(cpu, IMMEDIATE);   break;
        case 0x25: op_and(cpu, ZERO_PAGE);   break;
        case 0x35: op_and(cpu, ZERO_PAGE_X); break;
        case 0x2D: op_and(cpu, ABSOLUTE);    break;
        case 0x3D: op_and(cpu, ABSOLUTE_X);  break;
        case 0x39: op_and(cpu, ABSOLUTE_Y);  break;
        case 0x21: op_and(cpu, INDX_IND);    break;
        case 0x31: op_and(cpu, IND_INDX);    break;
        // ASL
        case 0x0A: op_asl(cpu, ACCUMULATOR); break;
        case 0x06: op_asl(cpu, ZERO_PAGE);   break;
        case 0x16: op_asl(cpu, ZERO_PAGE_X); break;
        case 0x0E: op_asl(cpu, ABSOLUTE);    break;
        case 0x1E: op_asl(cpu, ABSOLUTE_X);  break;
        // BCC
        case 0x90: op_bcc(cpu, RELATIVE);    break;
        // BCS
        case 0xB0: op_bcs(cpu, RELATIVE);    break;
        // BEQ
        case 0xF0: op_beq(cpu, RELATIVE);    break;
        // BIT
        case 0x24: op_bit(cpu, ZERO_PAGE);   break;
        case 0x2C: op_bit(cpu, ABSOLUTE);    break;
        // BMI
        case 0x30: op_bmi(cpu, RELATIVE);    break;
        // BNE
        case 0xD0: op_bne(cpu, RELATIVE);    break;
        // BPL
        case 0x10: op_bpl(cpu, RELATIVE);    break;
        // BRK
        case 0x00: op_brk(cpu, IMPLICIT);    break;
        // BVC
        case 0x50: op_bvc(cpu, RELATIVE);    break;
        // BVS
        case 0x70: op_bvs(cpu, RELATIVE);    break;
        // CLC
        case 0x18: op_clc(cpu, IMPLICIT);    break;
        // CLD
        case 0xD8: op_cld(cpu, IMPLICIT);    break;
        // CLI
        case 0x58: op_cli(cpu, IMPLICIT);    break;
        // CLV
        case 0xB8: op_clv(cpu, IMPLICIT);    break;
        // CMP
        case 0xC9: op_cmp(cpu, IMMEDIATE);   break;
        case 0xC5: op_cmp(cpu, ZERO_PAGE);   break;
        case 0xD5: op_cmp(cpu, ZERO_PAGE_X); break;
        case 0xCD: op_cmp(cpu, ABSOLUTE);    break;
        case 0xDD: op_cmp(cpu, ABSOLUTE_X);  break;
        case 0xD9: op_cmp(cpu, ABSOLUTE_Y);  break;
        case 0xC1: op_cmp(cpu, INDX_IND);    break;
        case 0xD1: op_cmp(cpu, IND_INDX);    break;
        // CMX
        case 0xE0: op_cmx(cpu, IMMEDIATE);   break;
        case 0xE4: op_cmx(cpu, ZERO_PAGE);   break;
        case 0xEC: op_cmx(cpu, ABSOLUTE);    break;
        // CMY
        case 0xC0: op_cmy(cpu, IMMEDIATE);   break;
        case 0xC4: op_cmy(cpu, ZERO_PAGE);   break;
        case 0xCC: op_cmy(cpu, ABSOLUTE);    break;
        // DEC
        case 0xC6: op_dec(cpu, ZERO_PAGE);   break;
        case 0xD6: op_dec(cpu, ZERO_PAGE_X); break;
        case 0xCE: op_dec(cpu, ABSOLUTE);    break;
        case 0xDE: op_dec(cpu, ABSOLUTE_X);  break;
        // DEX
        case 0xCA: op_dex(cpu, IMPLICIT);    break;
        // DEY
        case 0x88: op_dey(cpu, IMPLICIT);    break;
        // EOR
        case 0x49: op_eor(cpu, IMMEDIATE);   break;
        case 0x45: op_eor(cpu, ZERO_PAGE);   break;
        case 0x55: op_eor(cpu, ZERO_PAGE_X); break;
        case 0x4D: op_eor(cpu, ABSOLUTE);    break;
        case 0x5D: op_eor(cpu, ABSOLUTE_X);  break;
        case 0x59: op_eor(cpu, ABSOLUTE_Y);  break;
        case 0x41: op_eor(cpu, INDX_IND);    break;
        case 0x51: op_eor(cpu, IND_INDX);    break;
        // INC
        case 0xE6: op_inc(cpu, ZERO_PAGE);   break;
        case 0xF6: op_inc(cpu, ZERO_PAGE_X); break;
        case 0xEE: op_inc(cpu, ABSOLUTE);    break;
        case 0xFE: op_inc(cpu, ABSOLUTE_X);  break;
        // INX
        case 0xE8: op_inx(cpu, IMPLICIT);    break;
        // INY
        case 0xC8: op_iny(cpu, IMPLICIT);    break;
        // JMP
        case 0x4C: op_jmp(cpu, ABSOLUTE);    break;
        case 0x6C: op_jmp(cpu, INDIRECT);    break;
        // JSR
        case 0x20: op_jsr(cpu, ABSOLUTE);    break;
        // LDA
        case 0xA9: op_lda(cpu, IMMEDIATE);   break;
        case 0xA5: op_lda(cpu, ZERO_PAGE);   break;
        case 0xB5: op_lda(cpu, ZERO_PAGE_X); break;
        case 0xAD: op_lda(cpu, ABSOLUTE);    break;
        case 0xBD: op_lda(cpu, ABSOLUTE_X);  break;
        case 0xB9: op_lda(cpu, ABSOLUTE_Y);  break;
        case 0xA1: op_lda(cpu, INDX_IND);    break;
        case 0xB1: op_lda(cpu, IND_INDX);    break;
        // LDX
        case 0xA2: op_ldx(cpu, IMMEDIATE);   break;
        case 0xA6: op_ldx(cpu, ZERO_PAGE);   break;
        case 0xB6: op_ldx(cpu, ZERO_PAGE_Y); break;
        case 0xAE: op_ldx(cpu, ABSOLUTE);    break;
        case 0xBE: op_ldx(cpu, ABSOLUTE_Y);  break;
        // LDY
        case 0xA0: op_ldy(cpu, IMMEDIATE);   break;
        case 0xA4: op_ldy(cpu, ZERO_PAGE);   break;
        case 0xB4: op_ldy(cpu, ZERO_PAGE_X); break;
        case 0xAC: op_ldy(cpu, ABSOLUTE);    break;
        case 0xBC: op_ldy(cpu, ABSOLUTE_X);  break;
        // NOP
        case 0xEA: op_nop(cpu, IMPLICIT);    break;
        // ORA
        case 0x09: op_ora(cpu, IMMEDIATE);   break;
        case 0x05: op_ora(cpu, ZERO_PAGE);   break;
        case 0x15: op_ora(cpu, ZERO_PAGE_X); break;
        case 0x0D: op_ora(cpu, ABSOLUTE);    break;
        case 0x1D: op_ora(cpu, ABSOLUTE_X);  break;
        case 0x19: op_ora(cpu, ABSOLUTE_Y);  break;
        case 0x01: op_ora(cpu, INDX_IND);    break;
        case 0x11: op_ora(cpu, IND_INDX);    break;
        // PHA
        case 0x48: op_pha(cpu, IMPLICIT);    break;
        // PHP
        case 0x08: op_php(cpu, IMPLICIT);    break;
        // PLA
        case 0x68: op_pla(cpu, IMPLICIT);    break;
        // PHP
        case 0x28: op_plp(cpu, IMPLICIT);    break;
        // ROL
        case 0x2A: op_rol(cpu, ACCUMULATOR); break;
        case 0x26: op_rol(cpu, ZERO_PAGE);   break;
        case 0x36: op_rol(cpu, ZERO_PAGE_X); break;
        case 0x2E: op_rol(cpu, ABSOLUTE);    break;
        case 0x3E: op_rol(cpu, ABSOLUTE_X);  break;
        // ROR
        case 0x6A: op_ror(cpu, ACCUMULATOR); break;
        case 0x66: op_ror(cpu, ZERO_PAGE);   break;
        case 0x76: op_ror(cpu, ZERO_PAGE_X); break;
        case 0x6E: op_ror(cpu, ABSOLUTE);    break;
        case 0x7E: op_ror(cpu, ABSOLUTE_X);  break;
        // RTI
        case 0x40: op_rti(cpu, IMPLICIT);    break;
        // RTS
        case 0x60: op_rti(cpu, IMPLICIT);    break;
        // SBC
        case 0xE9: op_sbc(cpu, IMMEDIATE);   break;
        case 0xE5: op_sbc(cpu, ZERO_PAGE);   break;
        case 0xF5: op_sbc(cpu, ZERO_PAGE_X); break;
        case 0xED: op_sbc(cpu, ABSOLUTE);    break;
        case 0xFD: op_sbc(cpu, ABSOLUTE_X);  break;
        case 0xF9: op_sbc(cpu, ABSOLUTE_Y);  break;
        case 0xE1: op_sbc(cpu, INDX_IND);    break;
        case 0xF1: op_sbc(cpu, IND_INDX);    break;
        // SEC
        case 0x38: op_sec(cpu, IMPLICIT);    break;
        // SED
        case 0xF8: op_sed(cpu, IMPLICIT);    break;
        // SEI
        case 0x78: op_sei(cpu, IMPLICIT);    break;
        // STA
        case 0x85: op_sta(cpu, ZERO_PAGE);   break;
        case 0x95: op_sta(cpu, ZERO_PAGE_X); break;
        case 0x8D: op_sta(cpu, ABSOLUTE);    break;
        case 0x9D: op_sta(cpu, ABSOLUTE_X);  break;
        case 0x99: op_sta(cpu, ABSOLUTE_Y);  break;
        case 0x81: op_sta(cpu, INDX_IND);    break;
        case 0x91: op_sta(cpu, IND_INDX);    break;
        // STX
        case 0x86: op_stx(cpu, ZERO_PAGE);   break;
        case 0x96: op_stx(cpu, ZERO_PAGE_Y); break;
        case 0x8E: op_stx(cpu, ABSOLUTE);    break;
        // STY
        case 0x84: op_sty(cpu, ZERO_PAGE);   break;
        case 0x94: op_sty(cpu, ZERO_PAGE_X); break;
        case 0x8C: op_sty(cpu, ABSOLUTE);    break;
        // TAX
        case 0xAA: op_tax(cpu, IMPLICIT);    break;
        // TAY
        case 0xA8: op_tay(cpu, IMPLICIT);    break;
        // TSX
        case 0xBA: op_tsx(cpu, IMPLICIT);    break;
        // TXA
        case 0x8A: op_txa(cpu, IMPLICIT);    break;
        // TXS
        case 0x9A: op_txs(cpu, IMPLICIT);    break;
        // TYA
        case 0x98: op_tya(cpu, IMPLICIT);    break;
        default:
            fprintf(stderr, "Error: Invalid opcode: %02x\n", opcode);
            cpu->powered_on = false;
            break;
    }
}

// Instruction Implementations
void op_adc(CPU_t* cpu, AddrMode mode) {
    bool overflow;
    uint8_t rhs = *cpu_get_op_target(cpu, mode, true);
    uint16_t result = cpu->reg_A + rhs + get_bit(cpu->reg_P, stat_CARRY);

    cpu->reg_A = result;
    overflow = !((cpu->reg_A ^ rhs) & 0x80) && ((cpu->reg_A ^ result) & 0x80);

    cpu->reg_P = set_bit(cpu->reg_P, stat_CARRY, result > 255);
    cpu->reg_P = set_bit(cpu->reg_P, stat_ZERO, cpu->reg_A == 0);
    cpu->reg_P = set_bit(cpu->reg_P, stat_NEGATIVE, get_bit(cpu->reg_A, 7));
    cpu->reg_P = set_bit(cpu->reg_P, stat_OVERFLOW, overflow);
}

void op_and(CPU_t* cpu, AddrMode mode) {
    cpu->reg_A = cpu->reg_A & *cpu_get_op_target(cpu, mode, true);
    cpu->reg_P = set_bit(cpu->reg_P, stat_ZERO, cpu->reg_A == 0);
    cpu->reg_P = set_bit(cpu->reg_P, stat_NEGATIVE, get_bit(cpu->reg_A, 7));
}

void op_asl(CPU_t* cpu, AddrMode mode) {
    uint8_t* target = cpu_get_op_target(cpu, mode, false);
    bool old_bit_7 = get_bit(*target, 7);

    cpu_write_back(cpu, target, *target << 1);

    cpu->reg_P = set_bit(cpu->reg_P, stat_ZERO, *target == 0);
    cpu->reg_P = set_bit(cpu->reg_P, stat_NEGATIVE, get_bit(*target, 7));
    cpu->reg_P = set_bit(cpu->reg_P, stat_CARRY, old_bit_7);
}

void op_bcc(CPU_t* cpu, AddrMode mode) {
    if (!get_bit(cpu->reg_P, stat_CARRY)) {
        cpu_tick(cpu);
        cpu->reg_PC = cpu_address_from_mode(cpu, mode, true);
    }
}

void op_bcs(CPU_t* cpu, AddrMode mode) {
    if (get_bit(cpu->reg_P, stat_CARRY)) {
        cpu->reg_PC = cpu_address_from_mode(cpu, mode, true);
    }
}

void op_beq(CPU_t* cpu, AddrMode mode) {
    if (get_bit(cpu->reg_P, stat_ZERO)) {
        cpu->reg_PC = cpu_address_from_mode(cpu, mode, true);
    }
}

void op_bit(CPU_t* cpu, AddrMode mode) {
    uint8_t value = cpu_address_from_mode(cpu, mode, NULL);
    value = cpu->reg_A & value;

    cpu->reg_P = set_bit(cpu->reg_P, stat_ZERO, cpu->reg_A == 0);
    cpu->reg_P = set_bit(cpu->reg_P, stat_OVERFLOW, get_bit(value, 6));
    cpu->reg_P = set_bit(cpu->reg_P, stat_NEGATIVE, get_bit(value, 7));
}

void op_bmi(CPU_t* cpu, AddrMode mode) {
    bool page_cross = false;

    if (get_bit(cpu->reg_P, stat_NEGATIVE)) {
        cpu->reg_PC = cpu_address_from_mode(cpu, mode, page_cross);
    }
}

void op_bne(CPU_t* cpu, AddrMode mode) {
    bool page_cross = false;

    if (!get_bit(cpu->reg_P, stat_ZERO)) {
        cpu->reg_PC = cpu_address_from_mode(cpu, mode, page_cross);
    }
}

void op_bpl(CPU_t* cpu, AddrMode mode) {
    bool page_cross = false;

    if (!get_bit(cpu->reg_P, stat_NEGATIVE)) {
        cpu->reg_PC = cpu_address_from_mode(cpu, mode, page_cross);
    }
}

void op_brk(CPU_t* cpu, AddrMode mode) {
    uint8_t upper_PC = cpu->reg_PC >> 8;
    uint8_t lower_PC = cpu->reg_PC;
    uint8_t status = cpu->reg_P | 0b0011000; // Set bits 4 and 5
    cpu_stack_push(cpu, upper_PC);
    cpu_stack_push(cpu, lower_PC);
    cpu_stack_push(cpu, status);

    cpu->reg_PC = cpu_get_vector(cpu, BRK_VECTOR);
    cpu->reg_P = set_bit(cpu->reg_P, stat_INT, true);
}

void op_bvc(CPU_t* cpu, AddrMode mode) {
    bool page_cross = false;

    if (!get_bit(cpu->reg_P, stat_OVERFLOW)) {
        cpu->reg_PC = cpu_address_from_mode(cpu, mode, page_cross);
    }
}

void op_bvs(CPU_t* cpu, AddrMode mode) {
    bool page_cross = false;

    if (get_bit(cpu->reg_P, stat_OVERFLOW)) {
        cpu->reg_PC = cpu_address_from_mode(cpu, mode, page_cross);
    }
}

void op_clc(CPU_t* cpu, AddrMode mode) {
    cpu->reg_P = set_bit(cpu->reg_P, stat_CARRY, false);
}

void op_cld(CPU_t* cpu, AddrMode mode) {
    cpu->reg_P = set_bit(cpu->reg_P, stat_DECIMAL, false);
}

void op_cli(CPU_t* cpu, AddrMode mode) {
    cpu->reg_P = set_bit(cpu->reg_P, stat_INT, false);
}

void op_clv(CPU_t* cpu, AddrMode mode) {
    cpu->reg_P = set_bit(cpu->reg_P, stat_OVERFLOW, false);
}

void op_cmp(CPU_t* cpu, AddrMode mode) {
    uint8_t value, diff;
    bool page_cross = false;

    if (mode == IMMEDIATE) {
        value = *cpu_map_read(cpu, cpu->reg_PC++);
    } else {
        uint16_t address = cpu_address_from_mode(cpu, mode, page_cross);
        value = *cpu_map_read(cpu, address);
    }

    diff = cpu->reg_A - value;

    cpu->reg_P = set_bit(cpu->reg_P, stat_CARRY, cpu->reg_A >= value);
    cpu->reg_P = set_bit(cpu->reg_P, stat_ZERO, cpu->reg_A == value);
    cpu->reg_P = set_bit(cpu->reg_P, stat_NEGATIVE, get_bit(diff, 7));
}

void op_cmx(CPU_t* cpu, AddrMode mode) {
    uint8_t value, diff;
    bool page_cross = false;

    if (mode == IMMEDIATE) {
        value = *cpu_map_read(cpu, cpu->reg_PC++);
    } else {
        uint16_t address = cpu_address_from_mode(cpu, mode, page_cross);
        value = *cpu_map_read(cpu, address);
    }

    diff = cpu->reg_X - value;

    cpu->reg_P = set_bit(cpu->reg_P, stat_CARRY, cpu->reg_X >= value);
    cpu->reg_P = set_bit(cpu->reg_P, stat_ZERO, cpu->reg_X == value);
    cpu->reg_P = set_bit(cpu->reg_P, stat_NEGATIVE, get_bit(diff, 7));
}

void op_cmy(CPU_t* cpu, AddrMode mode) {
    uint8_t value, diff;
    bool page_cross = false;

    if (mode == IMMEDIATE) {
        value = *cpu_map_read(cpu, cpu->reg_PC++);
    } else {
        uint16_t address = cpu_address_from_mode(cpu, mode, page_cross);
        value = *cpu_map_read(cpu, address);
    }

    diff = cpu->reg_Y - value;

    cpu->reg_P = set_bit(cpu->reg_P, stat_CARRY, cpu->reg_Y >= value);
    cpu->reg_P = set_bit(cpu->reg_P, stat_ZERO, cpu->reg_Y == value);
    cpu->reg_P = set_bit(cpu->reg_P, stat_NEGATIVE, get_bit(diff, 7));
}

void op_dec(CPU_t* cpu, AddrMode mode) {
    bool page_cross = false;
    uint16_t address = cpu_address_from_mode(cpu, mode, page_cross);
    uint8_t value = *cpu_map_read(cpu, address) - 1;
    cpu_map_write(cpu, address, value);

    cpu->reg_P = set_bit(cpu->reg_P, stat_ZERO, value == 0);
    cpu->reg_P = set_bit(cpu->reg_P, stat_NEGATIVE, get_bit(value, 7));
}

void op_dex(CPU_t* cpu, AddrMode mode) {
    cpu->reg_X--;

    cpu->reg_P = set_bit(cpu->reg_P, stat_ZERO, cpu->reg_X == 0);
    cpu->reg_P = set_bit(cpu->reg_P, stat_NEGATIVE, get_bit(cpu->reg_X, 7));
}

void op_dey(CPU_t* cpu, AddrMode mode) {
    cpu->reg_Y--;

    cpu->reg_P = set_bit(cpu->reg_P, stat_ZERO, cpu->reg_Y == 0);
    cpu->reg_P = set_bit(cpu->reg_P, stat_NEGATIVE, get_bit(cpu->reg_Y, 7));
}

void op_eor(CPU_t* cpu, AddrMode mode) {
    bool page_cross = false;

    if (mode == IMMEDIATE) {
        cpu->reg_A = cpu->reg_A ^ *cpu_map_read(cpu, cpu->reg_PC++);
    } else {
        uint16_t address = cpu_address_from_mode(cpu, mode, page_cross);
        cpu->reg_A = cpu->reg_A ^ *cpu_map_read(cpu, address);
    }

    cpu->reg_P = set_bit(cpu->reg_P, stat_ZERO, cpu->reg_A == 0);
    cpu->reg_P = set_bit(cpu->reg_P, stat_NEGATIVE, get_bit(cpu->reg_A, 7));
}

void op_inc(CPU_t* cpu, AddrMode mode) {
    uint16_t address = cpu_address_from_mode(cpu, mode, NULL);
    uint8_t value = *cpu_map_read(cpu, address) + 1;
    cpu_map_write(cpu, address, value);

    cpu->reg_P = set_bit(cpu->reg_P, stat_ZERO, value == 0);
    cpu->reg_P = set_bit(cpu->reg_P, stat_NEGATIVE, get_bit(value, 7));
}

void op_inx(CPU_t* cpu, AddrMode mode) {
    cpu->reg_X++;

    cpu->reg_P = set_bit(cpu->reg_P, stat_ZERO, cpu->reg_X == 0);
    cpu->reg_P = set_bit(cpu->reg_P, stat_NEGATIVE, get_bit(cpu->reg_X, 7));
}

void op_iny(CPU_t* cpu, AddrMode mode) {
    cpu->reg_Y++;

    cpu->reg_P = set_bit(cpu->reg_P, stat_ZERO, cpu->reg_Y == 0);
    cpu->reg_P = set_bit(cpu->reg_P, stat_NEGATIVE, get_bit(cpu->reg_Y, 7));
}

void op_jmp(CPU_t* cpu, AddrMode mode) {
    uint16_t address = cpu_address_from_mode(cpu, mode, NULL);
    cpu->reg_PC = address;
    cpu_tick(cpu);
}

void op_jsr(CPU_t* cpu, AddrMode mode) {
    uint16_t address = cpu_address_from_mode(cpu, mode, NULL);
    uint16_t return_address = cpu->reg_PC - 1; // TODO: Figure this out...

    uint8_t upper_PC = return_address >> 8;
    uint8_t lower_PC = return_address;
    cpu_stack_push(cpu, upper_PC);
    cpu_stack_push(cpu, lower_PC);

    cpu->reg_PC = address;
}

void op_lda(CPU_t* cpu, AddrMode mode) {
    bool page_cross = false;

    if (mode == IMMEDIATE) {
        cpu->reg_A = *cpu_map_read(cpu, cpu->reg_PC++);
    } else {
        uint16_t address = cpu_address_from_mode(cpu, mode, page_cross);
        cpu->reg_A = *cpu_map_read(cpu, address);
    }

    cpu->reg_P = set_bit(cpu->reg_P, stat_ZERO, cpu->reg_A == 0);
    cpu->reg_P = set_bit(cpu->reg_P, stat_NEGATIVE, get_bit(cpu->reg_A, 7));
}

void op_ldx(CPU_t* cpu, AddrMode mode) {
    bool page_cross = false;

    if (mode == IMMEDIATE) {
        cpu->reg_X = *cpu_map_read(cpu, cpu->reg_PC++);
    } else {
        uint16_t address = cpu_address_from_mode(cpu, mode, page_cross);
        cpu->reg_X = *cpu_map_read(cpu, address);
    }

    cpu->reg_P = set_bit(cpu->reg_P, stat_ZERO, cpu->reg_X == 0);
    cpu->reg_P = set_bit(cpu->reg_P, stat_NEGATIVE, get_bit(cpu->reg_X, 7));
}

void op_ldy(CPU_t* cpu, AddrMode mode) {
    bool page_cross = false;

    if (mode == IMMEDIATE) {
        cpu->reg_Y = *cpu_map_read(cpu, cpu->reg_PC++);
    } else {
        uint16_t address = cpu_address_from_mode(cpu, mode, page_cross);
        cpu->reg_Y = *cpu_map_read(cpu, address);
    }

    cpu->reg_P = set_bit(cpu->reg_P, stat_ZERO, cpu->reg_Y == 0);
    cpu->reg_P = set_bit(cpu->reg_P, stat_NEGATIVE, get_bit(cpu->reg_Y, 7));
}

void op_nop(CPU_t* cpu, AddrMode mode) {
    cpu_tick(cpu);
}

void op_lsr(CPU_t* cpu, AddrMode mode) {
    bool old_bit_0;

    if (mode == ACCUMULATOR) {
        old_bit_0 = get_bit(cpu->reg_A, 0);
        cpu->reg_A = cpu->reg_A << 1;

        cpu->reg_P = set_bit(cpu->reg_P, stat_ZERO, cpu->reg_A == 0);
    } else {
        uint16_t address = cpu_address_from_mode(cpu, mode, NULL);
        uint8_t value = *cpu_map_read(cpu, address);
        old_bit_0 = get_bit(value, 0);
        cpu_map_write(cpu, address, value >> 1);

        cpu->reg_P = set_bit(cpu->reg_P, stat_ZERO, value == 0);
    }

    cpu->reg_P = set_bit(cpu->reg_P, stat_NEGATIVE, false);
    cpu->reg_P = set_bit(cpu->reg_P, stat_CARRY, old_bit_0);
}

void op_ora(CPU_t* cpu, AddrMode mode) {
    bool page_cross = false;

    if (mode == IMMEDIATE) {
        cpu->reg_A = cpu->reg_A | *cpu_map_read(cpu, cpu->reg_PC++);
    } else {
        uint16_t address = cpu_address_from_mode(cpu, mode, page_cross);
        cpu->reg_A = cpu->reg_A | *cpu_map_read(cpu, address);
    }

    cpu->reg_P = set_bit(cpu->reg_P, stat_ZERO, cpu->reg_A == 0);
    cpu->reg_P = set_bit(cpu->reg_P, stat_NEGATIVE, get_bit(cpu->reg_A, 7));
}

void op_pha(CPU_t* cpu, AddrMode mode) {
    cpu_stack_push(cpu, cpu->reg_A);
}

void op_php(CPU_t* cpu, AddrMode mode) {
    cpu_stack_push(cpu, cpu->reg_P | 0b00110000);
}

void op_pla(CPU_t* cpu, AddrMode mode) {
    cpu->reg_A = cpu_stack_pull(cpu);

    cpu->reg_P = set_bit(cpu->reg_P, stat_NEGATIVE, get_bit(cpu->reg_P, 7));
    cpu->reg_P = set_bit(cpu->reg_P, stat_ZERO, cpu->reg_P == 0);
}

void op_plp(CPU_t* cpu, AddrMode mode) {
    cpu->reg_P = cpu_stack_pull(cpu) & 0b11001111;
}

void op_rol(CPU_t* cpu, AddrMode mode) {
    bool old_carry = get_bit(cpu->reg_P, stat_CARRY);

    if (mode == ACCUMULATOR) {
        cpu->reg_P = set_bit(cpu->reg_P, stat_CARRY, get_bit(cpu->reg_A, 7));
        cpu->reg_A = (cpu->reg_A << 1) | old_carry;
        cpu->reg_P = set_bit(cpu->reg_P, stat_ZERO, cpu->reg_A == 0);
        cpu->reg_P = set_bit(cpu->reg_P, stat_NEGATIVE, get_bit(cpu->reg_A, 7));
    } else {
        uint16_t address = cpu_address_from_mode(cpu, mode, NULL);
        uint8_t value = (*cpu_map_read(cpu, address) << 1) | old_carry;
        cpu->reg_P = set_bit(cpu->reg_P, stat_CARRY, get_bit(value, 7));
        cpu->reg_P = set_bit(cpu->reg_P, stat_ZERO, value == 0);
        cpu->reg_P = set_bit(cpu->reg_P, stat_NEGATIVE, get_bit(value, 7));
        cpu_map_write(cpu, address, value);
    }
}

void op_ror(CPU_t* cpu, AddrMode mode) {
    bool old_bit_0, old_carry = get_bit(cpu->reg_P, stat_CARRY);

    if (mode == ACCUMULATOR) {
        old_bit_0 = get_bit(cpu->reg_A, 0);
        cpu->reg_A = set_bit(cpu->reg_A >> 1, 7, old_carry);
        cpu->reg_P = set_bit(cpu->reg_P, stat_CARRY, old_bit_0);
        cpu->reg_P = set_bit(cpu->reg_P, stat_ZERO, cpu->reg_A == 0);
        cpu->reg_P = set_bit(cpu->reg_P, stat_NEGATIVE, get_bit(cpu->reg_A, 7));
    } else {
        uint16_t address = cpu_address_from_mode(cpu, mode, NULL);
        uint8_t value = *cpu_map_read(cpu, address);
        old_bit_0 = get_bit(value, 0);
        value = set_bit(value >> 1, 7, old_carry);

        cpu->reg_P = set_bit(cpu->reg_P, stat_CARRY, old_bit_0);
        cpu->reg_P = set_bit(cpu->reg_P, stat_ZERO, value == 0);
        cpu->reg_P = set_bit(cpu->reg_P, stat_NEGATIVE, get_bit(value, 7));
        cpu_map_write(cpu, address, value);
    }
}

void op_rti(CPU_t* cpu, AddrMode mode) {
    cpu->reg_P = cpu_stack_pull(cpu);
    uint16_t new_PC = cpu_stack_pull(cpu);
    new_PC = (new_PC << 8) | cpu_stack_pull(cpu);
    cpu->reg_PC = new_PC;
}

void op_rts(CPU_t* cpu, AddrMode mode) {
    uint16_t new_PC = cpu_stack_pull(cpu);
    new_PC = (new_PC << 8) | cpu_stack_pull(cpu);
    cpu->reg_PC = new_PC + 1; // TODO: Confirm the addition
}

void op_sbc(CPU_t* cpu, AddrMode mode) {
    bool overflow, page_cross = false;
    uint8_t value;
    uint16_t result;

    if (mode == IMMEDIATE) {
        value = *cpu_map_read(cpu, cpu->reg_PC++);
    } else {
        uint16_t address = cpu_address_from_mode(cpu, mode, page_cross);
        value = *cpu_map_read(cpu, address);
    }

    result = cpu->reg_A - value - !get_bit(cpu->reg_P, stat_CARRY);
    cpu->reg_A = result;
    overflow = ((cpu->reg_A ^ result) & 0x80) && ((cpu->reg_A ^ value) & 0x80);

    cpu->reg_P = set_bit(cpu->reg_P, stat_ZERO, cpu->reg_A == 0);
    cpu->reg_P = set_bit(cpu->reg_P, stat_NEGATIVE, get_bit(cpu->reg_A, 7));
    cpu->reg_P = set_bit(cpu->reg_P, stat_CARRY, result < 0x100);
    cpu->reg_P = set_bit(cpu->reg_P, stat_OVERFLOW, overflow);
}

void op_sec(CPU_t* cpu, AddrMode mode) {
    cpu->reg_P = set_bit(cpu->reg_P, stat_CARRY, true);
}

void op_sed(CPU_t* cpu, AddrMode mode) {
    cpu->reg_P = set_bit(cpu->reg_P, stat_DECIMAL, true);
}

void op_sei(CPU_t* cpu, AddrMode mode) {
    cpu->reg_P = set_bit(cpu->reg_P, stat_INT, true);
}

void op_sta(CPU_t* cpu, AddrMode mode) {
    uint16_t address = cpu_address_from_mode(cpu, mode, NULL);
    cpu_map_write(cpu, address, cpu->reg_A);
}

void op_stx(CPU_t* cpu, AddrMode mode) {
    uint16_t address = cpu_address_from_mode(cpu, mode, NULL);
    cpu_map_write(cpu, address, cpu->reg_X);
}

void op_sty(CPU_t* cpu, AddrMode mode) {
    uint16_t address = cpu_address_from_mode(cpu, mode, NULL);
    cpu_map_write(cpu, address, cpu->reg_Y);
}

void op_tax(CPU_t* cpu, AddrMode mode) {
    cpu->reg_X = cpu->reg_A;

    cpu->reg_P = set_bit(cpu->reg_P, 7, cpu->reg_X == 0);
    cpu->reg_P = set_bit(cpu->reg_P, 7, get_bit(cpu->reg_X, 7));
}

void op_tay(CPU_t* cpu, AddrMode mode) {
    cpu->reg_Y = cpu->reg_A;

    cpu->reg_P = set_bit(cpu->reg_P, 7, cpu->reg_Y == 0);
    cpu->reg_P = set_bit(cpu->reg_P, 7, get_bit(cpu->reg_Y, 7));
}

void op_tsx(CPU_t* cpu, AddrMode mode) {
    cpu->reg_X = cpu->reg_S;

    cpu->reg_P = set_bit(cpu->reg_P, 7, cpu->reg_X == 0);
    cpu->reg_P = set_bit(cpu->reg_P, 7, get_bit(cpu->reg_X, 7));
}

void op_txa(CPU_t* cpu, AddrMode mode) {
    cpu->reg_A = cpu->reg_X;

    cpu->reg_P = set_bit(cpu->reg_P, 7, cpu->reg_A == 0);
    cpu->reg_P = set_bit(cpu->reg_P, 7, get_bit(cpu->reg_A, 7));
}

void op_txs(CPU_t* cpu, AddrMode mode) {
    cpu->reg_S = cpu->reg_X;
}

void op_tya(CPU_t* cpu, AddrMode mode) {
    cpu->reg_A = cpu->reg_Y;

    cpu->reg_P = set_bit(cpu->reg_P, 7, cpu->reg_A == 0);
    cpu->reg_P = set_bit(cpu->reg_P, 7, get_bit(cpu->reg_A, 7));
}

// Stack helpers
void cpu_stack_push(CPU_t* cpu, uint8_t value) {
    cpu_map_write(cpu, STACK_OFFSET | cpu->reg_S, value);
    cpu->reg_S--;
}

uint8_t cpu_stack_pull(CPU_t* cpu) {
    uint8_t value = *cpu_map_read(cpu, STACK_OFFSET | cpu->reg_S);
    cpu->reg_S++;
    return value;
}

// For multi-byte instructions
uint8_t* cpu_get_op_target(CPU_t* cpu, AddrMode mode, bool page_cross_cost) {
    uint16_t address;

    switch (mode) {
        case IMMEDIATE:
            return cpu_map_read(cpu, cpu->reg_PC++);
        case ACCUMULATOR:
            return &cpu->reg_A;
        default:
            address = cpu_address_from_mode(cpu, mode, page_cross_cost);
            return cpu_map_read(cpu, address);
    }
}

// Addressing mode implementations
uint16_t cpu_address_from_mode(CPU_t* cpu, AddrMode mode, bool page_cross_cost) {
    uint8_t arg1, arg2;
    int8_t arg1_signed;

    // For INDIRECT addressing
    uint16_t indirect_addr;
    uint8_t ind_arg1, ind_arg2;

    switch (mode) {
        case ZERO_PAGE:
            return *cpu_map_read(cpu, cpu->reg_PC++);
        case ZERO_PAGE_X:
            arg1 = *cpu_map_read(cpu, cpu->reg_PC++);
            return (uint8_t) (arg1 + cpu->reg_X); // Wrap around
        case ZERO_PAGE_Y:
            arg1 = *cpu_map_read(cpu, cpu->reg_PC++);
            return (uint8_t) (arg1 + cpu->reg_Y); // Wrap around
        case RELATIVE:
            arg1_signed = *cpu_map_read(cpu, cpu->reg_PC++);
            return ((int32_t) cpu->reg_PC) + arg1_signed;
        case ABSOLUTE:
            arg1 = *cpu_map_read(cpu, cpu->reg_PC++);
            arg2 = *cpu_map_read(cpu, cpu->reg_PC++);
            return (((uint16_t) arg2) << 8) | arg1;
        case ABSOLUTE_X:
            arg1 = *cpu_map_read(cpu, cpu->reg_PC++);
            arg2 = *cpu_map_read(cpu, cpu->reg_PC++);
            return ((((uint16_t) arg2) << 8) | arg1) + cpu->reg_X;
        case ABSOLUTE_Y:
            arg1 = *cpu_map_read(cpu, cpu->reg_PC++);
            arg2 = *cpu_map_read(cpu, cpu->reg_PC++);
            return ((((uint16_t) arg2) << 8) | arg1) + cpu->reg_Y;
        case INDIRECT:
            arg1 = *cpu_map_read(cpu, cpu->reg_PC++);
            arg2 = *cpu_map_read(cpu, cpu->reg_PC++);
            indirect_addr = (((uint16_t) arg1) << 8) | arg2;
            ind_arg1 = *cpu_map_read(cpu, indirect_addr);
            ind_arg2 = *cpu_map_read(cpu, indirect_addr + 1);
            return (((uint16_t) ind_arg2) << 8) | ind_arg1;
        case INDX_IND:
            arg1 = *cpu_map_read(cpu, cpu->reg_PC++);
            return *cpu_map_read(cpu, (uint8_t) (arg1 + cpu->reg_X)); // Wrap
        case IND_INDX:
            arg1 = *cpu_map_read(cpu, cpu->reg_PC++);
            return *cpu_map_read(cpu, arg1) + cpu->reg_Y;
        default:
            return 0xBABE;
    }
}

uint8_t* cpu_map_read(CPU_t* cpu, uint16_t address) {
    cpu_tick(cpu);

#ifdef DEBUG
    printf("$%04x READ %04x\n", cpu->reg_PC, address);
#endif

    // The 2KiB of system memory is mapped from $0000-$07FF, but it's also
    // mirrored to $0800-$1FFF 3 times
    if (address < 0x2000) {
        return &cpu->memory[address % CPU_MEMORY_SIZE];
    }

    // The PPU's 8 registers are mapped onto $2000-$2007, and mirrored through
    // $3FFF (so they repeat every 8 bytes)
    if (address >= 0x2000 && address < 0x4000) {
        switch(address % 8) {
            case 2:
                return &cpu->ppu->reg_PPUSTATUS;
            case 4:
                return &cpu->ppu->reg_OAMDATA;
            case 7:
                return &cpu->ppu->reg_PPUDATA;
            default:
                return 0;
        }
    }

    // NES APU and I/O registers
    if (address >= 0x4000 && address < 0x4017) {
        return 0;
    }

    // APU and I/O functionality that is usually disabled
    if (address >= 0x4018 && address < 0x401F) {
        return 0;
    }

    // Expansion RAM
    if (address >= 0x4020 && address < 0x5FFF) {
        return 0;
    }

    // Cartridge
    if (address >= 0x6000 && address < 0xFFFF) {
        return rom_map_read(cpu->cartridge, address);
    }

    return 0;
}

void cpu_write_back(CPU_t* cpu, uint8_t* address, uint8_t value) {
    cpu_tick(cpu);
    *address = value;
}

void cpu_map_write(CPU_t* cpu, uint16_t address, uint8_t value) {
    cpu_tick(cpu);

#ifdef DEBUG
    printf("$%04x WRIT %04x %02x\n", cpu->reg_PC, address, value);
#endif

    if (address < 0x2000) {
        cpu->memory[address % CPU_MEMORY_SIZE] = value;
        return;
    }

    if (address >= 0x2000 && address < 0x4000) {
        switch(address % 8) {
            case 0:
                cpu->ppu->reg_PPUCTRL = value;
                return;
            case 1:
                cpu->ppu->reg_PPUMASK = value;
                return;
            case 3:
                cpu->ppu->reg_OAMADDR = value;
                return;
            case 4:
                cpu->ppu->reg_OAMDATA = value;
                return;
            case 5:
                cpu->ppu->reg_PPUSCROLL = value;
                return;
            case 6:
                cpu->ppu->reg_PPUADDR = value;
                return;
            case 7:
                cpu->ppu->reg_PPUDATA = value;
                return;
            default:
                return;
        }
    }

    // NES APU and I/O registers
    if (address >= 0x4000 && address < 0x4017) {
        switch(address) {
            case 0x4014:
                cpu->ppu->reg_OAMDMA = value;
        }
    }

    // Expansion RAM
    if (address >= 0x4020 && address < 0x5FFF) {
        return;
    }

    // Cartridge SRAM
    if (address >= 0x6000 && address < 0x7FFF) {
        return;
    }
}

void cpu_print_regs(CPU_t* cpu) {
    printf("cpu\n");
    printf("\t->reg_A  %02x\n", cpu->reg_A);
    printf("\t->reg_X  %02x\n", cpu->reg_X);
    printf("\t->reg_Y  %02x\n", cpu->reg_Y);
    printf("\t->reg_PC %04x\n", cpu->reg_PC);
    printf("\t->reg_S  %02x\n", cpu->reg_S);
    printf("\t->reg_P  %02x\n", cpu->reg_P);
}
