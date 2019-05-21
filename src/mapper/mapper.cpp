#include "mapper.h"

using namespace std;

Mapper::Mapper(RAM *ram) {
    // TODO: Get it to work without the full address on Windows.
    ifstream rom ("../resources/nestest.nes", ifstream::binary);
    // ifstream rom ("E:/Documents/programming/nes-emu/resources/nestest.nes", ifstream::binary);
    if (!rom.is_open()) {
        cerr << "Failed to open ROM.\n";
        exit(1);
    }

    this->ram = ram;

    // Skip first 14 bits.
    char c;
    for (size_t i = 0; rom.get(c) && i < 15; i++) {
        if (c == 0x1a) {
            continue;
        }
    }

    // PRG ROM.
    for (size_t i = 0; rom.get(c) && i < 16384; i++) {
        ram->write(c, 0x8000 + i);
        ram->write(c, 0xC000 + i);
    }

    // CHR ROM
    for (size_t i = 0; rom.get(c) && i < 8192; i++) {
        ram->write(c, 0x6000 + i);
    }

    rom.close();
}

Mapper::~Mapper() {
}