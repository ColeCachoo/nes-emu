// map.cc
//
#include "map.h"

#include <fmt/format.h>

#include <fstream>

NESerror map(const std::string &rom_path, uint8_t ram[], uint8_t vram[]) {
    std::ifstream rom(rom_path, std::ifstream::binary);
    if (!rom.is_open()) {   // Failed to open ROM.
        return ERROR;
    }

    int prg_rom_size = 0;
    int chr_rom_size = 0;

    std::string identification = "";
    // Skip first 15 bits.
    char c;
    for (size_t i = 0; rom.get(c) && i < 15; i++) {
        // if (i == 0 || i == 1 || i == 2) {
        //     identification += c;
        // }
        // if (c == 0x1a) {
        //     fmt::print("{}\n", identification);
        //     continue;
        // }
        switch (i) {
        case 0: case 1: case 2:
            identification += c;
            break;
        case 4:
            prg_rom_size = int(c);
            break;
        case 5:
            chr_rom_size = int(c);
            break;
        default:
            continue;
        }
    }
    fmt::print("{}  {}\n", prg_rom_size, chr_rom_size);

    // PRG ROM.
    for (size_t i = 0; rom.get(c) && i < prg_rom_size * 16 * 1024; i++) {
        ram[0x8000 + i] = uint8_t(c);
        ram[0xc000 + i] = uint8_t(c);
    }

    // TODO: Maps to PPU.
    // CHR ROM
    for (size_t i = 0; rom.get(c) && i < chr_rom_size * 8 * 1024; i++) {
        // ram->write(c, 0x6000 + uint16_t(i));
        vram[0x0000 + i] = uint8_t(c);
    }

    rom.close();
    return SUCCESS;
}
