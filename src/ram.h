// ram.h : RAM class just gives an interface to access the array used as RAM.
//
#pragma once

#include <fmt/format.h>
#include <cstdint>
#include <vector>
#include <cassert>

class RAM {
public:
    // Allocates given amount of memory.
    RAM(size_t size = 0) {
        memory = std::vector<uint8_t>(size);
        // TODO: Just write a test or something instead of this?
        assert(memory.size() == size);
    }

    ~RAM() = default;

    /// Reads address from memory and returns its contents.
    uint8_t read(const uint16_t addr) const {
        return memory.at(addr);
    }

    /// Writes value to address.
    void write(const uint8_t val, uint16_t addr) {
        memory.at(addr) = val;
    }

private:
    /// The RAM.
    std::vector<uint8_t> memory;
};
