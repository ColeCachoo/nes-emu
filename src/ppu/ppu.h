// ppu.h : PPU class that carries out all functions related to PPU in the NES.
//
#pragma once

#include "ram.h"

#include <bitset>
#include <cstdint>
// #include <vector>

class PPU {
public:
    PPU(RAM *r);
    ~PPU() = default;
private:
    // Registers. Holds the address in RAM that contains their values.
    // Comments taken from:
    // http://wiki.nesdev.com/w/index.php/PPU_registers

    // 7  bit  0
    // ---- ----
    // VPHB SINN
    // |||| ||||
    // |||| ||++- Base nametable address
    // |||| ||    (0 = $2000; 1 = $2400; 2 = $2800; 3 = $2C00)
    // |||| |+--- VRAM address increment per CPU read/write of PPUDATA
    // |||| |     (0: add 1, going across; 1: add 32, going down)
    // |||| +---- Sprite pattern table address for 8x8 sprites
    // ||||       (0: $0000; 1: $1000; ignored in 8x16 mode)
    // |||+------ Background pattern table address (0: $0000; 1: $1000)
    // ||+------- Sprite size (0: 8x8 pixels; 1: 8x16 pixels)
    // |+-------- PPU master/slave select
    // |          (0: read backdrop from EXT pins; 1: output color on EXT pins)
    // +--------- Generate an NMI at the start of the
    //            vertical blanking interval (0: off; 1: on)
    const uint16_t PPU_CTRL = 0x2000;

    // 7  bit  0
    // ---- ----
    // BGRs bMmG
    // |||| ||||
    // |||| |||+- Greyscale (0: normal color, 1: produce a greyscale display)
    // |||| ||+-- 1: Show background in leftmost 8 pixels of screen, 0: Hide
    // |||| |+--- 1: Show sprites in leftmost 8 pixels of screen, 0: Hide
    // |||| +---- 1: Show background
    // |||+------ 1: Show sprites
    // ||+------- Emphasize red
    // |+-------- Emphasize green
    // +--------- Emphasize blue
    const uint16_t PPU_MASK = 0x2001;

    // 7  bit  0
    // ---- ----
    // VSO. ....
    // |||| ||||
    // |||+-++++- Least significant bits previously written into a PPU register
    // |||        (due to register not being updated for this address)
    // ||+------- Sprite overflow. The intent was for this flag to be set
    // ||         whenever more than eight sprites appear on a scanline, but a
    // ||         hardware bug causes the actual behavior to be more complicated
    // ||         and generate false positives as well as false negatives; see
    // ||         PPU sprite evaluation. This flag is set during sprite
    // ||         evaluation and cleared at dot 1 (the second dot) of the
    // ||         pre-render line.
    // |+-------- Sprite 0 Hit.  Set when a nonzero pixel of sprite 0 overlaps
    // |          a nonzero background pixel; cleared at dot 1 of the pre-render
    // |          line.  Used for raster timing.
    // +--------- Vertical blank has started (0: not in vblank; 1: in vblank).
    //            Set at dot 1 of line 241 (the line *after* the post-render
    //            line); cleared after reading $2002 and at dot 1 of the
    //            pre-render line.
    const uint16_t PPU_STATUS = 0x2002;

    const uint16_t OAM_ADDR   = 0x2003;
    const uint16_t OAM_DATA   = 0x2004;
    const uint16_t PPU_SCROLL = 0x2005;
    const uint16_t PPU_ADDR   = 0x2006;
    const uint16_t PPU_DATA   = 0x2007;
    const uint16_t OAM_DMA    = 0x4014;

    // Internal registers.
    std::bitset<15> v;
    std::bitset<15> t;
    std::bitset<3>  x;
    std::bitset<1>  w;

    // 16kB of VRAM.
    RAM vram;

    // 64kB of ram shared with CPU.
    RAM *ram;
};
