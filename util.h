#ifndef UTIL_H__
#define UTIL_H__

#include <stdint.h>
#include <stdbool.h>

uint8_t set_bit(uint8_t byte, uint8_t n, bool value);
bool get_bit(uint8_t byte, uint8_t n);

#endif
