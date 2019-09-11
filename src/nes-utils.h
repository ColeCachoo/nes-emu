// utility.h : Contains useful defines/functions.
//
#pragma once
#include <cstdint>
#include <string>
#include <fmt/format.h>

/// Gets number with only the n'th bit set. Bit position starts from 0.
#define BIT_POS(n) (1 << (n))

/// Sets bits with the position(s) given by flag(s).
/// Returns result of the operation with the bits set.
#define SET_BIT(word, pos) ((word) | (pos))
// constexpr uint8_t set_bit(uint8_t word, uint8_t pos) { return word | pos; }

/// Clears bits with the position(s) given by flag(s).
/// Returns result of the operation with the bits cleared.
#define CLEAR_BIT(word, pos) ((word) & (~(pos)))
// constexpr uint8_t clear_bit(uint8_t word, uint8_t pos) { return word & (~pos); }

/// Gets higher 8 bits from given word.
// template<typename T>
// constexpr uint8_t high_byte(uint16_t word) { return word >> (sizeof(T) * 8 - 8); }
constexpr uint8_t high_byte(uint16_t word) { return uint8_t(word >> 8); }

/// Gets lower 8 bits from given word.
// template<typename T>
constexpr uint8_t low_byte(uint16_t word) { return uint8_t(word); }
