// ppu.cpp : Definitions for ppu.h
//
#include "ppu/ppu.h"

using namespace std;

PPU::PPU(RAM *r) {
    // Shared RAM.
    ram = r;

    // 16kB of memory.
    vram = RAM(16 * 1024);
}

// PPU::~PPU() {
// }