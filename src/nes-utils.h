// utility.h : Contains useful defines/functions.
//
#pragma once
#include <cstdint>
#include <string>
#include <fmt/format.h>

/// Gets number with only the n'th bit set. Bit position starts from 0.
#define bit_pos(n) (1 << (n))

/// Sets bits with the position(s) given by flag(s).
/// Returns result of the operation with the bits set.
#define set_bit(num, pos) ((num) | (pos))
/// Clears bits with the position(s) given by flag(s).
/// Returns result of the operation with the bits cleared.
#define clear_bit(num, pos) ((num) & (~(pos)))

/// Gets higher 8 bits from given number.
template<typename T>
constexpr uint8_t high_byte(T num)
{
    return num >> (sizeof(T) * 8 - 8);
}

/// Gets lower 8 bits from given number.
template<typename T>
constexpr uint8_t low_byte(T num)
{
    return (uint8_t) num;
}
