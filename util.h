#ifndef UTIL_H__
#define UTIL_H__

#include <stdint.h>
#include <stdbool.h>

inline uint8_t set_bit(uint8_t byte, uint8_t n, bool value);
inline bool get_bit(uint8_t byte, uint8_t n);
void print_data(uint8_t* start, uint16_t num_bytes);

#endif
