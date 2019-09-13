// utility.h : Contains useful defines/functions.
//
#pragma once

#include <fmt/format.h>

#include <cstdint>
#include <string>

/// Gets number with only the n'th bit set. Bit position starts from 0.
template <typename T>
constexpr T bit_pos(T n) { return 1 << n; }

/// Sets bits with the position(s) given by flag(s).
/// Returns result of the operation with the bits set.
template <typename T>
constexpr T set_bit(T word, T pos) { return word | pos; }

/// Clears bits with the position(s) given by flag(s).
/// Returns result of the operation with the bits cleared.
template <typename T>
constexpr T clear_bit(T word, T pos) { return word & (~pos); }

/// Gets higher 8 bits from given word.
// template<typename T>
// constexpr uint8_t high_byte(uint16_t word) { return word >> (sizeof(T) * 8 - 8); }
constexpr uint8_t high_byte(uint16_t word) { return uint8_t(word >> 8); }

/// Gets lower 8 bits from given word.
// template<typename T>
constexpr uint8_t low_byte(uint16_t word) { return uint8_t(word); }
