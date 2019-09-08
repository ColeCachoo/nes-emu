// map.cpp
//
#include "map.h"

#include <fstream>

NESerror map(const char *rom_path, uint8_t *ram) {
    std::ifstream rom(rom_path, std::ifstream::binary);
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
    for (size_t i = 0; rom.get(c) && i < 16 * 1024; i++) {
        ram[0x8000 + i] = (uint8_t) c;
        ram[0xc000 + i] = (uint8_t) c;
    }

    // TODO: This maps to vram.
    // CHR ROM
    // for (size_t i = 0; rom.get(c) && i < 8 * 1024; i++) {
        // ram->write(c, 0x6000 + uint16_t(i));
        // ram[0x6000 + i] = (uint8_t) c;
    // }

    rom.close();
    return SUCCESS;
}
