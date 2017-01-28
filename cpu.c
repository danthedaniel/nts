#include <stdint.h>
#include <time.h>
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
    cpu->reg_PC = 0x8000; // TODO: Confirm this

    cpu->cycle = 0;

    // Connect hardware
    cpu->ppu = ppu_init();
    cpu->apu = apu_init();
    cpu->cartridge = cartridge;
    cpu->cartridge_bank = 0;

    return cpu;
}

void cpu_free(CPU_t* cpu) {
    free(cpu->ppu);
    free(cpu->apu);
    free(cpu);
}

void cpu_start(CPU_t* cpu) {
    uint8_t cycle_count;
    struct timespec delay;
    delay.tv_sec = 0;
    cpu->powered_on = true;

    while (cpu->powered_on) {
        cycle_count = cpu_cycle(cpu);
        cpu->cycle += cycle_count;

        // TODO: Confirm casting shenanigans
        delay.tv_nsec = (long) cycle_count * (NS_PER_CLOCK) ;

        if (!nanosleep(&delay, NULL)) {
            printf("Thread recieved interrupt\n");
            return;
        }
    }

    printf("CPU shutting down\n");
}

uint8_t cpu_cycle(CPU_t* cpu) {
    printf("$%04x EXEC", cpu->reg_PC);
    uint8_t opcode = cpu_map_read(cpu, cpu->reg_PC++);
    printf("%02x\n", opcode);

    switch(opcode) {
        // ADC
        case 0x69: return 2 + op_adc(cpu, IMMEDIATE);
        case 0x65: return 3 + op_adc(cpu, ZERO_PAGE);
        case 0x75: return 4 + op_adc(cpu, ZERO_PAGE_X);
        case 0x6D: return 4 + op_adc(cpu, ABSOLUTE);
        case 0x7D: return 4 + op_adc(cpu, ABSOLUTE_X);
        case 0x79: return 4 + op_adc(cpu, ABSOLUTE_Y);
        case 0x61: return 6 + op_adc(cpu, INDX_IND);
        case 0x71: return 5 + op_adc(cpu, IND_INDX);
        // AND
        case 0x29: return 2 + op_and(cpu, IMMEDIATE);
        case 0x25: return 3 + op_and(cpu, ZERO_PAGE);
        case 0x35: return 4 + op_and(cpu, ZERO_PAGE_X);
        case 0x2D: return 4 + op_and(cpu, ABSOLUTE);
        case 0x3D: return 4 + op_and(cpu, ABSOLUTE_X);
        case 0x39: return 4 + op_and(cpu, ABSOLUTE_Y);
        case 0x21: return 6 + op_and(cpu, INDX_IND);
        case 0x31: return 5 + op_and(cpu, IND_INDX);
        // ASL
        case 0x0A: return 2 + op_asl(cpu, ACCUMULATOR);
        case 0x06: return 5 + op_asl(cpu, ZERO_PAGE);
        case 0x16: return 6 + op_asl(cpu, ZERO_PAGE_X);
        case 0x0E: return 6 + op_asl(cpu, ABSOLUTE);
        case 0x1E: return 7 + op_asl(cpu, ABSOLUTE_X);
        // BCC
        case 0x90: return 2 + op_bcc(cpu, RELATIVE);
        // BCS
        case 0xB0: return 2 + op_bcs(cpu, RELATIVE);
        // BEQ
        case 0xF0: return 2 + op_beq(cpu, RELATIVE);
        // BIT
        case 0x24: return 2 + op_bit(cpu, ZERO_PAGE);
        case 0x2C: return 3 + op_bit(cpu, ABSOLUTE);
        // BMI
        case 0x30: return 2 + op_bmi(cpu, RELATIVE);
        // BNE
        case 0xD0: return 2 + op_bne(cpu, RELATIVE);
        // BPL
        case 0x10: return 2 + op_bpl(cpu, RELATIVE);
        // BRK
        case 0x00: return 7 + op_brk(cpu, IMPLICIT);
        // BVC
        case 0x50: return 2 + op_bvc(cpu, RELATIVE);
        // BVS
        case 0x70: return 2 + op_bvs(cpu, RELATIVE);
        // CLC
        case 0x18: return 2 + op_clc(cpu, IMPLICIT);
        // CLD
        case 0xD8: return 2 + op_cld(cpu, IMPLICIT);
        // CLI
        case 0x58: return 2 + op_cli(cpu, IMPLICIT);
        // CLV
        case 0xB8: return 2 + op_clv(cpu, IMPLICIT);
        // CMP
        case 0xC9: return 2 + op_cmp(cpu, IMMEDIATE);
        case 0xC5: return 3 + op_cmp(cpu, ZERO_PAGE);
        case 0xD5: return 4 + op_cmp(cpu, ZERO_PAGE_X);
        case 0xCD: return 4 + op_cmp(cpu, ABSOLUTE);
        case 0xDD: return 4 + op_cmp(cpu, ABSOLUTE_X);
        case 0xD9: return 4 + op_cmp(cpu, ABSOLUTE_Y);
        case 0xC1: return 6 + op_cmp(cpu, INDX_IND);
        case 0xD1: return 5 + op_cmp(cpu, IND_INDX);
        // CMX
        case 0xE0: return 2 + op_cmx(cpu, IMMEDIATE);
        case 0xE4: return 3 + op_cmx(cpu, ZERO_PAGE);
        case 0xEC: return 4 + op_cmx(cpu, ABSOLUTE);
        // CMY
        case 0xC0: return 2 + op_cmy(cpu, IMMEDIATE);
        case 0xC4: return 3 + op_cmy(cpu, ZERO_PAGE);
        case 0xCC: return 4 + op_cmy(cpu, ABSOLUTE);
        // DEC
        case 0xC6: return 5 + op_dec(cpu, ZERO_PAGE);
        case 0xD6: return 5 + op_dec(cpu, ZERO_PAGE_X);
        case 0xCE: return 5 + op_dec(cpu, ABSOLUTE);
        case 0xDE: return 5 + op_dec(cpu, ABSOLUTE_X);
        // DEX
        case 0xCA: return 2 + op_dex(cpu, IMPLICIT);
        // DEY
        case 0x88: return 2 + op_dey(cpu, IMPLICIT);
        // EOR
        case 0x49: return 2 + op_eor(cpu, IMMEDIATE);
        case 0x45: return 3 + op_eor(cpu, ZERO_PAGE);
        case 0x55: return 4 + op_eor(cpu, ZERO_PAGE_X);
        case 0x4D: return 4 + op_eor(cpu, ABSOLUTE);
        case 0x5D: return 4 + op_eor(cpu, ABSOLUTE_X);
        case 0x59: return 4 + op_eor(cpu, ABSOLUTE_Y);
        case 0x41: return 6 + op_eor(cpu, INDX_IND);
        case 0x51: return 5 + op_eor(cpu, IND_INDX);
        // INC
        case 0xE6: return 5 + op_inc(cpu, ZERO_PAGE);
        case 0xF6: return 6 + op_inc(cpu, ZERO_PAGE_X);
        case 0xEE: return 6 + op_inc(cpu, ABSOLUTE);
        case 0xFE: return 7 + op_inc(cpu, ABSOLUTE_X);
        // INX
        case 0xE8: return 2 + op_inx(cpu, IMPLICIT);
        // INY
        case 0xC8: return 2 + op_iny(cpu, IMPLICIT);
        // JMP
        case 0x4C: return 3 + op_jmp(cpu, ABSOLUTE);
        case 0x6C: return 5 + op_jmp(cpu, INDIRECT);
        // JSR
        case 0x20: return 6 + op_jmp(cpu, ABSOLUTE);
        // LDA
        case 0xA9: return 2 + op_lda(cpu, IMMEDIATE);
        case 0xA5: return 3 + op_lda(cpu, ZERO_PAGE);
        case 0xB5: return 4 + op_lda(cpu, ZERO_PAGE_X);
        case 0xAD: return 4 + op_lda(cpu, ABSOLUTE);
        case 0xBD: return 4 + op_lda(cpu, ABSOLUTE_X);
        case 0xB9: return 4 + op_lda(cpu, ABSOLUTE_Y);
        case 0xA1: return 6 + op_lda(cpu, INDX_IND);
        case 0xB1: return 5 + op_lda(cpu, IND_INDX);
        // LDX
        case 0xA2: return 2 + op_ldx(cpu, IMMEDIATE);
        case 0xA6: return 3 + op_ldx(cpu, ZERO_PAGE);
        case 0xB6: return 4 + op_ldx(cpu, ZERO_PAGE_Y);
        case 0xAE: return 4 + op_ldx(cpu, ABSOLUTE);
        case 0xBE: return 4 + op_ldx(cpu, ABSOLUTE_Y);
        // LDY
        case 0xA0: return 2 + op_ldy(cpu, IMMEDIATE);
        case 0xA4: return 3 + op_ldy(cpu, ZERO_PAGE);
        case 0xB4: return 4 + op_ldy(cpu, ZERO_PAGE_X);
        case 0xAC: return 4 + op_ldy(cpu, ABSOLUTE);
        case 0xBC: return 4 + op_ldy(cpu, ABSOLUTE_X);
        // NOP
        case 0xEA: return 2;
        // ORA
        case 0x09: return 2 + op_ora(cpu, IMMEDIATE);
        case 0x05: return 3 + op_ora(cpu, ZERO_PAGE);
        case 0x15: return 4 + op_ora(cpu, ZERO_PAGE_X);
        case 0x0D: return 4 + op_ora(cpu, ABSOLUTE);
        case 0x1D: return 4 + op_ora(cpu, ABSOLUTE_X);
        case 0x19: return 4 + op_ora(cpu, ABSOLUTE_Y);
        case 0x01: return 6 + op_ora(cpu, INDX_IND);
        case 0x11: return 5 + op_ora(cpu, IND_INDX);
        // PHA
        case 0x48: return 3 + op_pha(cpu, IMPLICIT);
        // PHP
        case 0x08: return 3 + op_php(cpu, IMPLICIT);
        // PLA
        case 0x68: return 4 + op_pla(cpu, IMPLICIT);
        // PHP
        case 0x28: return 4 + op_plp(cpu, IMPLICIT);
        // ROL // TODO: Continue from here
        case 0x2A: return 2 + op_rol(cpu, ACCUMULATOR);
        case 0x26: return 5 + op_rol(cpu, ZERO_PAGE);
        case 0x36: return 6 + op_rol(cpu, ZERO_PAGE_X);
        case 0x2E: return 6 + op_rol(cpu, ABSOLUTE);
        case 0x3E: return 7 + op_rol(cpu, ABSOLUTE_X);
        // ROR
        case 0x6A: return 2 + op_ror(cpu, ACCUMULATOR);
        case 0x66: return 5 + op_ror(cpu, ZERO_PAGE);
        case 0x76: return 6 + op_ror(cpu, ZERO_PAGE_X);
        case 0x6E: return 6 + op_ror(cpu, ABSOLUTE);
        case 0x7E: return 7 + op_ror(cpu, ABSOLUTE_X);
        // RTI
        case 0x40: return 6 + op_rti(cpu, IMPLICIT);
        // RTS
        case 0x60: return 6 + op_rti(cpu, IMPLICIT);
        // SBC
        case 0xE9: return 2 + op_sbc(cpu, IMMEDIATE);
        case 0xE5: return 3 + op_sbc(cpu, ZERO_PAGE);
        case 0xF5: return 4 + op_sbc(cpu, ZERO_PAGE_X);
        case 0xED: return 4 + op_sbc(cpu, ABSOLUTE);
        case 0xFD: return 4 + op_sbc(cpu, ABSOLUTE_X);
        case 0xF9: return 4 + op_sbc(cpu, ABSOLUTE_Y);
        case 0xE1: return 6 + op_sbc(cpu, INDX_IND);
        case 0xF1: return 5 + op_sbc(cpu, IND_INDX);
        // SEC
        case 0x38: return 2 + op_sec(cpu, IMPLICIT);
        // SED
        case 0xF8: return 2 + op_sed(cpu, IMPLICIT);
        // SEI
        case 0x78: return 2 + op_sei(cpu, IMPLICIT);
        // STA
        case 0x85: return 2 + op_sta(cpu, ZERO_PAGE);
        case 0x95: return 4 + op_sta(cpu, ZERO_PAGE_X);
        case 0x8D: return 4 + op_sta(cpu, ABSOLUTE);
        case 0x9D: return 5 + op_sta(cpu, ABSOLUTE_X);
        case 0x99: return 5 + op_sta(cpu, ABSOLUTE_Y);
        case 0x81: return 6 + op_sta(cpu, INDX_IND);
        case 0x91: return 6 + op_sta(cpu, IND_INDX);
        // STX
        case 0x86: return 3 + op_stx(cpu, ZERO_PAGE);
        case 0x96: return 4 + op_stx(cpu, ZERO_PAGE_Y);
        case 0x8E: return 4 + op_stx(cpu, ABSOLUTE);
        // STY
        case 0x84: return 3 + op_sty(cpu, ZERO_PAGE);
        case 0x94: return 4 + op_sty(cpu, ZERO_PAGE_X);
        case 0x8C: return 4 + op_sty(cpu, ABSOLUTE);
        // TAX
        case 0xAA: return 2 + op_tax(cpu, IMPLICIT);
        // TAY
        case 0xA8: return 2 + op_tay(cpu, IMPLICIT);
        // TSX
        case 0xBA: return 2 + op_tsx(cpu, IMPLICIT);
        // TXA
        case 0x8A: return 2 + op_txa(cpu, IMPLICIT);
        // TXS
        case 0x9A: return 2 + op_txs(cpu, IMPLICIT);
        // TYA
        case 0x98: return 2 + op_tya(cpu, IMPLICIT);
        default:
            fprintf(stderr, "Error: Invalid opcode: %x", opcode);
            cpu->powered_on = false;
            return 0;
    }
}

// Instruction Implementations
uint8_t op_adc(CPU_t* cpu, AddrMode mode) {
    uint16_t result;
    uint8_t rhs;
    bool overflow, page_cross = false;

    if (mode == IMMEDIATE) {
        rhs = cpu_map_read(cpu, cpu->reg_PC++);
    } else {
        uint16_t address = cpu_address_from_mode(cpu, mode, &page_cross);
        rhs = cpu_map_read(cpu, address);
    }

    result = cpu->reg_A + rhs + get_bit(cpu->reg_P, stat_CARRY);
    cpu->reg_A = result;
    overflow = !((cpu->reg_A ^ rhs) & 0x80) && ((cpu->reg_A ^ result) & 0x80);

    cpu->reg_P = set_bit(cpu->reg_P, stat_CARRY, result > 255);
    cpu->reg_P = set_bit(cpu->reg_P, stat_ZERO, cpu->reg_A == 0);
    cpu->reg_P = set_bit(cpu->reg_P, stat_NEGATIVE, get_bit(cpu->reg_A, 7));
    cpu->reg_P = set_bit(cpu->reg_P, stat_OVERFLOW, overflow);

    return 0 + page_cross;
}

uint8_t op_and(CPU_t* cpu, AddrMode mode) {
    bool page_cross = false;

    if (mode == IMMEDIATE) {
        cpu->reg_A = cpu->reg_A & cpu_map_read(cpu, cpu->reg_PC++);
    } else {
        uint16_t address = cpu_address_from_mode(cpu, mode, &page_cross);
        cpu->reg_A = cpu->reg_A & cpu_map_read(cpu, address);
    }

    cpu->reg_P = set_bit(cpu->reg_P, stat_ZERO, cpu->reg_A == 0);
    cpu->reg_P = set_bit(cpu->reg_P, stat_NEGATIVE, get_bit(cpu->reg_A, 7));

    return page_cross;
}

uint8_t op_asl(CPU_t* cpu, AddrMode mode) {
    bool old_bit_7, page_cross = false;

    if (mode == ACCUMULATOR) {
        old_bit_7 = get_bit(cpu->reg_A, 7);
        cpu->reg_A = cpu->reg_A << 1;

        cpu->reg_P = set_bit(cpu->reg_P, stat_ZERO, cpu->reg_A == 0);
        cpu->reg_P = set_bit(cpu->reg_P, stat_NEGATIVE, get_bit(cpu->reg_A, 7));
    } else {
        uint16_t address = cpu_address_from_mode(cpu, mode, &page_cross);
        uint8_t value = cpu_map_read(cpu, address);
        old_bit_7 = get_bit(value, 7);
        cpu_map_write(cpu, address, value << 1);

        cpu->reg_P = set_bit(cpu->reg_P, stat_ZERO, value == 0);
        cpu->reg_P = set_bit(cpu->reg_P, stat_NEGATIVE, get_bit(value, 7));
    }

    cpu->reg_P = set_bit(cpu->reg_P, stat_CARRY, old_bit_7);

    return 0 + page_cross;
}

uint8_t op_bcc(CPU_t* cpu, AddrMode mode) {
    bool page_cross = false;

    if (!get_bit(cpu->reg_P, stat_CARRY)) {
        cpu->reg_PC = cpu_address_from_mode(cpu, mode, &page_cross);

        return 1 + page_cross;
    }

    return 0;
}

uint8_t op_bcs(CPU_t* cpu, AddrMode mode) {
    bool page_cross = false;

    if (get_bit(cpu->reg_P, stat_CARRY)) {
        cpu->reg_PC = cpu_address_from_mode(cpu, mode, &page_cross);

        return 1 + page_cross;
    }

    return 0;
}

uint8_t op_beq(CPU_t* cpu, AddrMode mode) {
    bool page_cross = false;

    if (get_bit(cpu->reg_P, stat_ZERO)) {
        cpu->reg_PC = cpu_address_from_mode(cpu, mode, &page_cross);

        return 1 + page_cross;
    }

    return 0;
}

uint8_t op_bit(CPU_t* cpu, AddrMode mode) {
    uint8_t value = cpu_address_from_mode(cpu, mode, NULL);
    value = cpu->reg_A & value;

    cpu->reg_P = set_bit(cpu->reg_P, stat_ZERO, cpu->reg_A == 0);
    cpu->reg_P = set_bit(cpu->reg_P, stat_OVERFLOW, get_bit(value, 6));
    cpu->reg_P = set_bit(cpu->reg_P, stat_NEGATIVE, get_bit(value, 7));

    return 0;
}

uint8_t op_bmi(CPU_t* cpu, AddrMode mode) {
    bool page_cross = false;

    if (get_bit(cpu->reg_P, stat_NEGATIVE)) {
        cpu->reg_PC = cpu_address_from_mode(cpu, mode, &page_cross);

        return 1 + page_cross;
    }

    return 0;
}

uint8_t op_bne(CPU_t* cpu, AddrMode mode) {
    bool page_cross = false;

    if (!get_bit(cpu->reg_P, stat_ZERO)) {
        cpu->reg_PC = cpu_address_from_mode(cpu, mode, &page_cross);

        return 1 + page_cross;
    }

    return 0;
}

uint8_t op_bpl(CPU_t* cpu, AddrMode mode) {
    bool page_cross = false;

    if (!get_bit(cpu->reg_P, stat_NEGATIVE)) {
        cpu->reg_PC = cpu_address_from_mode(cpu, mode, &page_cross);

        return 1 + page_cross;
    }

    return 0;
}

uint8_t op_brk(CPU_t* cpu, AddrMode mode) {
    uint8_t upper_PC = cpu->reg_PC >> 8;
    uint8_t lower_PC = cpu->reg_PC;
    uint8_t status = cpu->reg_P | 0b0011000; // Set bits 4 and 5
    cpu_stack_push(cpu, upper_PC);
    cpu_stack_push(cpu, lower_PC);
    cpu_stack_push(cpu, status);

    uint16_t new_PC = ((uint16_t) cpu_map_read(cpu, 0xFFFE)) << 8;
    cpu->reg_PC = new_PC | cpu_map_read(cpu, 0xFFFF);
    cpu->reg_P = set_bit(cpu->reg_P, stat_INT, true);

    return 0;
}

uint8_t op_bvc(CPU_t* cpu, AddrMode mode) {
    bool page_cross = false;

    if (!get_bit(cpu->reg_P, stat_OVERFLOW)) {
        cpu->reg_PC = cpu_address_from_mode(cpu, mode, &page_cross);

        return 1 + page_cross;
    }

    return 0;
}

uint8_t op_bvs(CPU_t* cpu, AddrMode mode) {
    bool page_cross = false;

    if (get_bit(cpu->reg_P, stat_OVERFLOW)) {
        cpu->reg_PC = cpu_address_from_mode(cpu, mode, &page_cross);

        return 1 + page_cross;
    }

    return 0;
}

uint8_t op_clc(CPU_t* cpu, AddrMode mode) {
    cpu->reg_P = set_bit(cpu->reg_P, stat_CARRY, false);
    return 0;
}

uint8_t op_cld(CPU_t* cpu, AddrMode mode) {
    cpu->reg_P = set_bit(cpu->reg_P, stat_DECIMAL, false);
    return 0;
}

uint8_t op_cli(CPU_t* cpu, AddrMode mode) {
    cpu->reg_P = set_bit(cpu->reg_P, stat_INT, false);
    return 0;
}

uint8_t op_clv(CPU_t* cpu, AddrMode mode) {
    cpu->reg_P = set_bit(cpu->reg_P, stat_OVERFLOW, false);
    return 0;
}

uint8_t op_cmp(CPU_t* cpu, AddrMode mode) {
    uint8_t value, diff;
    bool page_cross = false;

    if (mode == IMMEDIATE) {
        value = cpu_map_read(cpu, cpu->reg_PC++);
    } else {
        uint16_t address = cpu_address_from_mode(cpu, mode, &page_cross);
        value = cpu_map_read(cpu, address);
    }

    diff = cpu->reg_A - value;

    cpu->reg_P = set_bit(cpu->reg_P, stat_CARRY, cpu->reg_A >= value);
    cpu->reg_P = set_bit(cpu->reg_P, stat_ZERO, cpu->reg_A == value);
    cpu->reg_P = set_bit(cpu->reg_P, stat_NEGATIVE, get_bit(diff, 7));

    return page_cross;
}

uint8_t op_cmx(CPU_t* cpu, AddrMode mode) {
    uint8_t value, diff;
    bool page_cross = false;

    if (mode == IMMEDIATE) {
        value = cpu_map_read(cpu, cpu->reg_PC++);
    } else {
        uint16_t address = cpu_address_from_mode(cpu, mode, &page_cross);
        value = cpu_map_read(cpu, address);
    }

    diff = cpu->reg_X - value;

    cpu->reg_P = set_bit(cpu->reg_P, stat_CARRY, cpu->reg_X >= value);
    cpu->reg_P = set_bit(cpu->reg_P, stat_ZERO, cpu->reg_X == value);
    cpu->reg_P = set_bit(cpu->reg_P, stat_NEGATIVE, get_bit(diff, 7));

    return page_cross;
}

uint8_t op_cmy(CPU_t* cpu, AddrMode mode) {
    uint8_t value, diff;
    bool page_cross = false;

    if (mode == IMMEDIATE) {
        value = cpu_map_read(cpu, cpu->reg_PC++);
    } else {
        uint16_t address = cpu_address_from_mode(cpu, mode, &page_cross);
        value = cpu_map_read(cpu, address);
    }

    diff = cpu->reg_Y - value;

    cpu->reg_P = set_bit(cpu->reg_P, stat_CARRY, cpu->reg_Y >= value);
    cpu->reg_P = set_bit(cpu->reg_P, stat_ZERO, cpu->reg_Y == value);
    cpu->reg_P = set_bit(cpu->reg_P, stat_NEGATIVE, get_bit(diff, 7));

    return page_cross;
}

uint8_t op_dec(CPU_t* cpu, AddrMode mode) {
    bool page_cross = false;
    uint16_t address = cpu_address_from_mode(cpu, mode, &page_cross);
    uint8_t value = cpu_map_read(cpu, address) - 1;
    cpu_map_write(cpu, address, value);

    cpu->reg_P = set_bit(cpu->reg_P, stat_ZERO, value == 0);
    cpu->reg_P = set_bit(cpu->reg_P, stat_NEGATIVE, get_bit(value, 7));

    return page_cross;
}

uint8_t op_dex(CPU_t* cpu, AddrMode mode) {
    cpu->reg_X--;

    cpu->reg_P = set_bit(cpu->reg_P, stat_ZERO, cpu->reg_X == 0);
    cpu->reg_P = set_bit(cpu->reg_P, stat_NEGATIVE, get_bit(cpu->reg_X, 7));

    return 0;
}

uint8_t op_dey(CPU_t* cpu, AddrMode mode) {
    cpu->reg_Y--;

    cpu->reg_P = set_bit(cpu->reg_P, stat_ZERO, cpu->reg_Y == 0);
    cpu->reg_P = set_bit(cpu->reg_P, stat_NEGATIVE, get_bit(cpu->reg_Y, 7));

    return 0;
}

uint8_t op_eor(CPU_t* cpu, AddrMode mode) {
    bool page_cross = false;

    if (mode == IMMEDIATE) {
        cpu->reg_A = cpu->reg_A ^ cpu_map_read(cpu, cpu->reg_PC++);
    } else {
        uint16_t address = cpu_address_from_mode(cpu, mode, &page_cross);
        cpu->reg_A = cpu->reg_A ^ cpu_map_read(cpu, address);
    }

    cpu->reg_P = set_bit(cpu->reg_P, stat_ZERO, cpu->reg_A == 0);
    cpu->reg_P = set_bit(cpu->reg_P, stat_NEGATIVE, get_bit(cpu->reg_A, 7));

    return page_cross;
}

uint8_t op_inc(CPU_t* cpu, AddrMode mode) {
    uint16_t address = cpu_address_from_mode(cpu, mode, NULL);
    uint8_t value = cpu_map_read(cpu, address) + 1;
    cpu_map_write(cpu, address, value);

    cpu->reg_P = set_bit(cpu->reg_P, stat_ZERO, value == 0);
    cpu->reg_P = set_bit(cpu->reg_P, stat_NEGATIVE, get_bit(value, 7));

    return 0;
}

uint8_t op_inx(CPU_t* cpu, AddrMode mode) {
    cpu->reg_X++;

    cpu->reg_P = set_bit(cpu->reg_P, stat_ZERO, cpu->reg_X == 0);
    cpu->reg_P = set_bit(cpu->reg_P, stat_NEGATIVE, get_bit(cpu->reg_X, 7));

    return 0;
}

uint8_t op_iny(CPU_t* cpu, AddrMode mode) {
    cpu->reg_Y++;

    cpu->reg_P = set_bit(cpu->reg_P, stat_ZERO, cpu->reg_Y == 0);
    cpu->reg_P = set_bit(cpu->reg_P, stat_NEGATIVE, get_bit(cpu->reg_Y, 7));

    return 0;
}

uint8_t op_jmp(CPU_t* cpu, AddrMode mode) {
    uint16_t address = cpu_address_from_mode(cpu, mode, NULL);
    cpu->reg_PC = address;

    return 0;
}

uint8_t op_jsr(CPU_t* cpu, AddrMode mode) {
    uint16_t address = cpu_address_from_mode(cpu, mode, NULL);
    uint16_t return_address = cpu->reg_PC - 1; // TODO: Figure this out...

    uint8_t upper_PC = return_address >> 8;
    uint8_t lower_PC = return_address;
    cpu_stack_push(cpu, upper_PC);
    cpu_stack_push(cpu, lower_PC);

    cpu->reg_PC = address;

    return 0;
}

uint8_t op_lda(CPU_t* cpu, AddrMode mode) {
    bool page_cross = false;

    if (mode == IMMEDIATE) {
        cpu->reg_A = cpu_map_read(cpu, cpu->reg_PC++);
    } else {
        uint16_t address = cpu_address_from_mode(cpu, mode, &page_cross);
        cpu->reg_A = cpu_map_read(cpu, address);
    }

    cpu->reg_P = set_bit(cpu->reg_P, stat_ZERO, cpu->reg_A == 0);
    cpu->reg_P = set_bit(cpu->reg_P, stat_NEGATIVE, get_bit(cpu->reg_A, 7));

    return 0;
}

uint8_t op_ldx(CPU_t* cpu, AddrMode mode) {
    bool page_cross = false;

    if (mode == IMMEDIATE) {
        cpu->reg_X = cpu_map_read(cpu, cpu->reg_PC++);
    } else {
        uint16_t address = cpu_address_from_mode(cpu, mode, &page_cross);
        cpu->reg_X = cpu_map_read(cpu, address);
    }

    cpu->reg_P = set_bit(cpu->reg_P, stat_ZERO, cpu->reg_X == 0);
    cpu->reg_P = set_bit(cpu->reg_P, stat_NEGATIVE, get_bit(cpu->reg_X, 7));

    return 0;
}

uint8_t op_ldy(CPU_t* cpu, AddrMode mode) {
    bool page_cross = false;

    if (mode == IMMEDIATE) {
        cpu->reg_Y = cpu_map_read(cpu, cpu->reg_PC++);
    } else {
        uint16_t address = cpu_address_from_mode(cpu, mode, &page_cross);
        cpu->reg_Y = cpu_map_read(cpu, address);
    }

    cpu->reg_P = set_bit(cpu->reg_P, stat_ZERO, cpu->reg_Y == 0);
    cpu->reg_P = set_bit(cpu->reg_P, stat_NEGATIVE, get_bit(cpu->reg_Y, 7));

    return 0;
}

uint8_t op_lsr(CPU_t* cpu, AddrMode mode) {
    bool old_bit_0;

    if (mode == ACCUMULATOR) {
        old_bit_0 = get_bit(cpu->reg_A, 0);
        cpu->reg_A = cpu->reg_A << 1;

        cpu->reg_P = set_bit(cpu->reg_P, stat_ZERO, cpu->reg_A == 0);
    } else {
        uint16_t address = cpu_address_from_mode(cpu, mode, NULL);
        uint8_t value = cpu_map_read(cpu, address);
        old_bit_0 = get_bit(value, 0);
        cpu_map_write(cpu, address, value >> 1);

        cpu->reg_P = set_bit(cpu->reg_P, stat_ZERO, value == 0);
    }

    cpu->reg_P = set_bit(cpu->reg_P, stat_NEGATIVE, false);
    cpu->reg_P = set_bit(cpu->reg_P, stat_CARRY, old_bit_0);

    return 0;
}

uint8_t op_ora(CPU_t* cpu, AddrMode mode) {
    bool page_cross = false;

    if (mode == IMMEDIATE) {
        cpu->reg_A = cpu->reg_A | cpu_map_read(cpu, cpu->reg_PC++);
    } else {
        uint16_t address = cpu_address_from_mode(cpu, mode, &page_cross);
        cpu->reg_A = cpu->reg_A | cpu_map_read(cpu, address);
    }

    cpu->reg_P = set_bit(cpu->reg_P, stat_ZERO, cpu->reg_A == 0);
    cpu->reg_P = set_bit(cpu->reg_P, stat_NEGATIVE, get_bit(cpu->reg_A, 7));

    return page_cross;
}

uint8_t op_pha(CPU_t* cpu, AddrMode mode) {
    cpu_stack_push(cpu, cpu->reg_A);

    return 0;
}

uint8_t op_php(CPU_t* cpu, AddrMode mode) {
    cpu_stack_push(cpu, cpu->reg_P | 0b00110000);

    return 0;
}

uint8_t op_pla(CPU_t* cpu, AddrMode mode) {
    cpu->reg_A = cpu_stack_pull(cpu);

    cpu->reg_P = set_bit(cpu->reg_P, stat_NEGATIVE, get_bit(cpu->reg_P, 7));
    cpu->reg_P = set_bit(cpu->reg_P, stat_ZERO, cpu->reg_P == 0);

    return 0;
}

uint8_t op_plp(CPU_t* cpu, AddrMode mode) {
    cpu->reg_P = cpu_stack_pull(cpu) & 0b11001111;

    return 0;
}

uint8_t op_rol(CPU_t* cpu, AddrMode mode) {
    bool old_carry = get_bit(cpu->reg_P, stat_CARRY);

    if (mode == ACCUMULATOR) {
        cpu->reg_P = set_bit(cpu->reg_P, stat_CARRY, get_bit(cpu->reg_A, 7));
        cpu->reg_A = (cpu->reg_A << 1) | old_carry;
        cpu->reg_P = set_bit(cpu->reg_P, stat_ZERO, cpu->reg_A == 0);
        cpu->reg_P = set_bit(cpu->reg_P, stat_NEGATIVE, get_bit(cpu->reg_A, 7));
    } else {
        uint16_t address = cpu_address_from_mode(cpu, mode, NULL);
        uint8_t value = (cpu_map_read(cpu, address) << 1) | old_carry;
        cpu->reg_P = set_bit(cpu->reg_P, stat_CARRY, get_bit(value, 7));
        cpu->reg_P = set_bit(cpu->reg_P, stat_ZERO, value == 0);
        cpu->reg_P = set_bit(cpu->reg_P, stat_NEGATIVE, get_bit(value, 7));
        cpu_map_write(cpu, address, value);
    }

    return 0;
}

uint8_t op_ror(CPU_t* cpu, AddrMode mode) {
    bool old_bit_0, old_carry = get_bit(cpu->reg_P, stat_CARRY);

    if (mode == ACCUMULATOR) {
        old_bit_0 = get_bit(cpu->reg_A, 0);
        cpu->reg_A = set_bit(cpu->reg_A >> 1, 7, old_carry);
        cpu->reg_P = set_bit(cpu->reg_P, stat_CARRY, old_bit_0);
        cpu->reg_P = set_bit(cpu->reg_P, stat_ZERO, cpu->reg_A == 0);
        cpu->reg_P = set_bit(cpu->reg_P, stat_NEGATIVE, get_bit(cpu->reg_A, 7));
    } else {
        uint16_t address = cpu_address_from_mode(cpu, mode, NULL);
        uint8_t value = cpu_map_read(cpu, address);
        old_bit_0 = get_bit(value, 0);
        value = set_bit(value >> 1, 7, old_carry);

        cpu->reg_P = set_bit(cpu->reg_P, stat_CARRY, old_bit_0);
        cpu->reg_P = set_bit(cpu->reg_P, stat_ZERO, value == 0);
        cpu->reg_P = set_bit(cpu->reg_P, stat_NEGATIVE, get_bit(value, 7));
        cpu_map_write(cpu, address, value);
    }

    return 0;
}

uint8_t op_rti(CPU_t* cpu, AddrMode mode) {
    cpu->reg_P = cpu_stack_pull(cpu);
    uint16_t new_PC = cpu_stack_pull(cpu);
    new_PC = (new_PC << 8) | cpu_stack_pull(cpu);
    cpu->reg_PC = new_PC;

    return 0;
}

uint8_t op_rts(CPU_t* cpu, AddrMode mode) {
    uint16_t new_PC = cpu_stack_pull(cpu);
    new_PC = (new_PC << 8) | cpu_stack_pull(cpu);
    cpu->reg_PC = new_PC + 1; // TODO: Confirm the addition

    return 0;
}

uint8_t op_sbc(CPU_t* cpu, AddrMode mode) {
    bool overflow, page_cross = false;
    uint8_t value;
    uint16_t result;

    if (mode == IMMEDIATE) {
        value = cpu_map_read(cpu, cpu->reg_PC++);
    } else {
        uint16_t address = cpu_address_from_mode(cpu, mode, &page_cross);
        value = cpu_map_read(cpu, address);
    }

    result = cpu->reg_A - value - !get_bit(cpu->reg_P, stat_CARRY);
    cpu->reg_A = result;
    overflow = ((cpu->reg_A ^ result) & 0x80) && ((cpu->reg_A ^ value) & 0x80);

    cpu->reg_P = set_bit(cpu->reg_P, stat_ZERO, cpu->reg_A == 0);
    cpu->reg_P = set_bit(cpu->reg_P, stat_NEGATIVE, get_bit(cpu->reg_A, 7));
    cpu->reg_P = set_bit(cpu->reg_P, stat_CARRY, result < 0x100);
    cpu->reg_P = set_bit(cpu->reg_P, stat_OVERFLOW, overflow);

    return page_cross;
}

uint8_t op_sec(CPU_t* cpu, AddrMode mode) {
    cpu->reg_P = set_bit(cpu->reg_P, stat_CARRY, true);
    return 0;
}

uint8_t op_sed(CPU_t* cpu, AddrMode mode) {
    cpu->reg_P = set_bit(cpu->reg_P, stat_DECIMAL, true);
    return 0;
}

uint8_t op_sei(CPU_t* cpu, AddrMode mode) {
    cpu->reg_P = set_bit(cpu->reg_P, stat_INT, true);
    return 0;
}

uint8_t op_sta(CPU_t* cpu, AddrMode mode) {
    uint16_t address = cpu_address_from_mode(cpu, mode, NULL);
    cpu_map_write(cpu, address, cpu->reg_A);

    return 0;
}

uint8_t op_stx(CPU_t* cpu, AddrMode mode) {
    uint16_t address = cpu_address_from_mode(cpu, mode, NULL);
    cpu_map_write(cpu, address, cpu->reg_X);

    return 0;
}

uint8_t op_sty(CPU_t* cpu, AddrMode mode) {
    uint16_t address = cpu_address_from_mode(cpu, mode, NULL);
    cpu_map_write(cpu, address, cpu->reg_Y);

    return 0;
}

uint8_t op_tax(CPU_t* cpu, AddrMode mode) {
    cpu->reg_X = cpu->reg_A;

    cpu->reg_P = set_bit(cpu->reg_P, 7, cpu->reg_X == 0);
    cpu->reg_P = set_bit(cpu->reg_P, 7, get_bit(cpu->reg_X, 7));

    return 0;
}

uint8_t op_tay(CPU_t* cpu, AddrMode mode) {
    cpu->reg_Y = cpu->reg_A;

    cpu->reg_P = set_bit(cpu->reg_P, 7, cpu->reg_Y == 0);
    cpu->reg_P = set_bit(cpu->reg_P, 7, get_bit(cpu->reg_Y, 7));

    return 0;
}

uint8_t op_tsx(CPU_t* cpu, AddrMode mode) {
    cpu->reg_X = cpu->reg_S;

    cpu->reg_P = set_bit(cpu->reg_P, 7, cpu->reg_X == 0);
    cpu->reg_P = set_bit(cpu->reg_P, 7, get_bit(cpu->reg_X, 7));

    return 0;
}

uint8_t op_txa(CPU_t* cpu, AddrMode mode) {
    cpu->reg_A = cpu->reg_X;

    cpu->reg_P = set_bit(cpu->reg_P, 7, cpu->reg_A == 0);
    cpu->reg_P = set_bit(cpu->reg_P, 7, get_bit(cpu->reg_A, 7));

    return 0;
}

uint8_t op_txs(CPU_t* cpu, AddrMode mode) {
    cpu->reg_S = cpu->reg_X;

    return 0;
}

uint8_t op_tya(CPU_t* cpu, AddrMode mode) {
    cpu->reg_A = cpu->reg_Y;

    cpu->reg_P = set_bit(cpu->reg_P, 7, cpu->reg_A == 0);
    cpu->reg_P = set_bit(cpu->reg_P, 7, get_bit(cpu->reg_A, 7));

    return 0;
}


void cpu_stack_push(CPU_t* cpu, uint8_t value) {
    cpu_map_write(cpu, STACK_OFFSET | cpu->reg_S, value);
    cpu->reg_S--;
}

uint8_t cpu_stack_pull(CPU_t* cpu) {
    uint8_t value = cpu_map_read(cpu, STACK_OFFSET | cpu->reg_S);
    cpu->reg_S++;
    return value;
}

// Addressing mode implementations
uint16_t cpu_address_from_mode(CPU_t* cpu, AddrMode mode, bool* page_cross) {
    uint16_t ret;
    uint8_t arg1, arg2;
    int8_t arg1_signed;

    // For INDIRECT addressing
    uint16_t indirect_addr;
    uint8_t ind_arg1, ind_arg2;

    switch (mode) {
        case ZERO_PAGE:
            arg1 = cpu_map_read(cpu, cpu->reg_PC++);
            ret = (uint16_t) arg1;
            break;
        case ZERO_PAGE_X:
            arg1 = cpu_map_read(cpu, cpu->reg_PC++);
            ret = (uint8_t) (arg1 + cpu->reg_X); // Zero page wrap around
            break;
        case ZERO_PAGE_Y:
            arg1 = cpu_map_read(cpu, cpu->reg_PC++);
            ret = (uint8_t) (arg1 + cpu->reg_Y); // Zero page wrap around
            break;
        case RELATIVE:
            arg1_signed = cpu_map_read(cpu, cpu->reg_PC++);
            ret = ((int32_t) cpu->reg_PC) + arg1_signed;
            break;
        case ABSOLUTE:
            arg1 = cpu_map_read(cpu, cpu->reg_PC++);
            arg2 = cpu_map_read(cpu, cpu->reg_PC++);
            ret = (((uint16_t) arg1) << 8) | arg2;
            break;
        case ABSOLUTE_X:
            arg1 = cpu_map_read(cpu, cpu->reg_PC++);
            arg2 = cpu_map_read(cpu, cpu->reg_PC++);
            ret = ((((uint16_t) arg1) << 8) | arg2) + cpu->reg_X;
            break;
        case ABSOLUTE_Y:
            arg1 = cpu_map_read(cpu, cpu->reg_PC++);
            arg2 = cpu_map_read(cpu, cpu->reg_PC++);
            ret = ((((uint16_t) arg1) << 8) | arg2) + cpu->reg_Y;
            break;
        case INDIRECT:
            arg1 = cpu_map_read(cpu, cpu->reg_PC++);
            arg2 = cpu_map_read(cpu, cpu->reg_PC++);
            indirect_addr = (((uint16_t) arg1) << 8) | arg2;
            ind_arg1 = cpu_map_read(cpu, indirect_addr);
            ind_arg2 = cpu_map_read(cpu, indirect_addr + 1);
            ret = (((uint16_t) ind_arg2) << 8) | ind_arg1;
            break;
        case INDX_IND:
            arg1 = cpu_map_read(cpu, cpu->reg_PC++);
            ind_arg1 = cpu_map_read(cpu, (uint8_t) (arg1 + cpu->reg_X));
            ret = ind_arg1;
            break;
        case IND_INDX:
            arg1 = cpu_map_read(cpu, cpu->reg_PC++);
            ind_arg1 = cpu_map_read(cpu, arg1) + cpu->reg_Y;
            ret = ind_arg1;
            break;
        default:
            return 0;
    }

    if (page_cross != NULL)
        *page_cross = false; // TODO: Set page cross

    return ret;
}

uint8_t cpu_map_read(CPU_t* cpu, uint16_t address) {
    printf("$%04x READ %04x\n", cpu->reg_PC, address);
    // The 2KiB of system memory is mapped from $0000-$07FF, but it's also
    // mirrored to $0800-$1FFF 3 times
    if (address < 0x2000) {
        return cpu->memory[address % CPU_MEMORY_SIZE];
    }

    // The PPU's 8 registers are mapped onto $2000-$2007, and mirrored through
    // $3FFF (so they repeat every 8 bytes)
    if (address >= 0x2000 && address < 0x4000) {
        switch(address % 8) {
            case 2:
                return cpu->ppu->reg_PPUSTATUS;
                break;
            case 4:
                return cpu->ppu->reg_OAMDATA;
                break;
            case 7:
                return cpu->ppu->reg_PPUDATA;
                break;
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

    // Cartridge SRAM
    if (address >= 0x6000 && address < 0x7FFF) {
        return 0;
    }

    // Cartridge PrgROM
    if (address >= 0x8000 && address < 0xFFFF) {
        uint32_t prg_offset = (cpu->cartridge_bank * (PRG_PAGE_SIZE));
        return cpu->cartridge->prg_data[prg_offset + (address - 0x8000)];
    }

    return 0;
}

void cpu_map_write(CPU_t* cpu, uint16_t address, uint8_t value) {
    printf("$%04x WRIT %04x %02x\n", cpu->reg_PC, address, value);

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
