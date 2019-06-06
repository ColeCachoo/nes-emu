// map.cpp : Reads data from ROM and maps it to RAM.
// Definitions for mapper.h
//
#include "map.h"
#include "error.h"

#include <fmt/format.h>

#include <fstream>

using namespace std;

error_t
map(const string &rom_path, RAM &ram) {
    ifstream rom (rom_path, ifstream::binary);
    if (!rom.is_open()) {   // Failed to open ROM.
        return ERROR;
    }

    // Skip first 15 bits.
    char c;
    for (size_t i = 0; rom.get(c) && i < 15; i++) {
        if (c == 0x1a) {
            continue;
        }
    }

    // PRG ROM.
    for (size_t i = 0; rom.get(c) && i < 16384; i++) {
        ram.write(c, 0x8000 + uint16_t(i));
        ram.write(c, 0xC000 + uint16_t(i));
    }

    // CHR ROM
    for (size_t i = 0; rom.get(c) && i < 8192; i++) {
        ram.write(c, 0x6000 + uint16_t(i));
    }

    rom.close();
    return SUCCESS;
}
