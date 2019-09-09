// utility.h : Contains useful defines/functions.
//
#pragma once
#include <cstdint>
#include <string>
#include <fmt/format.h>

/// Gets number with only the n'th bit set. Bit position starts from 0.
// #define bit_pos(n) (1 << (n))
template<typename T>
constexpr auto bit_pos(T n) { return 1 << n; }

/// Sets bits with the position(s) given by flag(s).
/// Returns result of the operation with the bits set.
// #define set_bit(word, pos) ((word) | (pos))
template<typename W, typename P>
constexpr W set_bit(W word, P pos) { return word | pos; }
/// Clears bits with the position(s) given by flag(s).
/// Returns result of the operation with the bits cleared.
// #define clear_bit(word, pos) ((word) & (~(pos)))
template<typename W, typename P>
constexpr W clear_bit(W word, P pos) { return word & (~pos); }

/// Gets higher 8 bits from given word.
template<typename T>
constexpr uint8_t high_byte(T word) { return word >> (sizeof(T) * 8 - 8); }

/// Gets lower 8 bits from given word.
template<typename T>
constexpr uint8_t low_byte(T word) { return uint8_t(word); }
