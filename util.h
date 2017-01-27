#ifndef UTIL_H__
#define UTIL_H__

#include <stdint.h>
#include <stdbool.h>

uint8_t set_bit(uint8_t byte, uint8_t n, bool value);
bool get_bit(uint8_t byte, uint8_t n);
uint16_t set_bit(uint16_t half_word, uint8_t n, bool value);
bool get_bit(uint16_t half_word, uint8_t n);

#endif
