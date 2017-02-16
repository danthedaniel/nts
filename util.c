#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "util.h"

inline uint8_t set_bit(uint8_t byte, uint8_t n, bool value) {
    uint8_t mask = 1 << n;
    if (value)
        return byte | mask;
    else
        return byte & (~mask);
}

inline bool get_bit(uint8_t byte, uint8_t n) {
    return (byte << (7 - n)) >> 7;
}

void print_data(uint8_t* start, uint16_t num_bytes) {
    for (uint16_t i; i < num_bytes; ++i) {
        if (i % 16 == 0)
            printf("\n");
        printf("%02x ", start[i]);
    }

    printf("\n\n");
}
