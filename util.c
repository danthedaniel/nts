#include <stdint.h>
#include <stdbool.h>
#include "util.h"

uint8_t set_bit(uint8_t byte, uint8_t n, bool value) {
    uint8_t mask = 1 << n;
    if (value) {
        return byte | mask;
    } else {
        return byte & (~mask);
    }
}

bool get_bit(uint8_t byte, uint8_t n) {
    return (byte << (7 - n)) >> 7;
}
