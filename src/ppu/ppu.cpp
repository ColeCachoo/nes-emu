// ppu.cpp
//
#include "ppu/ppu.hpp"
#include "nesutils.hpp"

// Detailed comments taken from:
// http://wiki.nesdev.com/w/index.php/PPU_programmer_reference

namespace ppu {

enum { // Flags for PPUCTRL.
    NAMETABLE_0         = 1 << 0,   // (N) Nametable select (bit position 0).
    NAMETABLE_1         = 1 << 1,   // (N) Nametable select (bit position 1).
    INCREMENT           = 1 << 2,   // (I) Increment mode.
    SPRITE_TILE         = 1 << 3,   // (S) Sprite tile select.
    BACKGROUND_TILE     = 1 << 4,   // (B) Background tile select.
    SPRITE_SIZE         = 1 << 5,   // (H) Sprite size.
    PPU_MASTER_SLAVE    = 1 << 6,   // (P) PPU master/slave.
    NMI_ENABLE          = 1 << 7,   // (V) NMI Enable.
};

enum { // Flags for PPUMASK.
    GREYSCALE           = 1 << 0,   // (G) Greyscale.
    BACKGROUND_LEFT     = 1 << 1,   // (m) Background left column enable.
    SPRITE_LEFT         = 1 << 2,   // (M) Sprite left column enable.
    BACKGROUND_ENABLE   = 1 << 3,   // (b) Background enable.
    SPRITE_ENABLE       = 1 << 4,   // (s) Sprite enable.
    COLOR_R             = 1 << 5,   // (R) Color emphasis (red).
    COLOR_G             = 1 << 6,   // (G) Color emphasis (green).
    COLOR_B             = 1 << 7,   // (B) Color emphasis (blue).
};

enum { // Flags for PPUSTATUS.
    SPRITE_OVERFLOW     = 1 << 5,   // (O) Sprite overflow.
    SPRITE_HIT          = 1 << 6,   // (S) Sprite 0 hit.
    VBLANK              = 1 << 7,   // (V) vblank.
};

PPU::PPU(uint8_t *ram)
{
    ppu_ctrl    = &ram[0x2000];
    ppu_mask    = &ram[0x2001];
    ppu_status  = &ram[0x2002];
    oam_addr    = &ram[0x2003];
    oam_data    = &ram[0x2004];
    ppu_scroll  = &ram[0x2005];
    ppu_addr    = &ram[0x2006];
    ppu_data    = &ram[0x2007];
    oam_dma     = &ram[0x4014];

    *ppu_ctrl    = 0x00;
    *ppu_mask    = 0x00;
    *ppu_status  = 0xa0;
    *oam_addr    = 0x00;
    *oam_data    = 0x00;
    *ppu_scroll  = 0x00;
    *ppu_addr    = 0x00;
    *ppu_data    = 0x00;
    *oam_dma     = 0x00;

    vram = std::make_unique<uint8_t[]>(16 * 1024);
    oam = std::make_unique<uint8_t[]>(256);
}

void PPU::fetch()
{
    uint8_t val = *ppu_addr;
    if (*ppu_ctrl & INCREMENT) {
        *ppu_addr += 32;
    } else {
        *ppu_addr += 1;
    }

}

void PPU::run()
{
    // Start of vertical blanking?
    *ppu_status = set_bit(*ppu_status, VBLANK);
    *ppu_ctrl = set_bit(*ppu_ctrl, NMI_ENABLE);

    // End of vertical blanking?
    *ppu_ctrl = clear_bit(*ppu_ctrl, NMI_ENABLE);

    // Read ppu_status: return old status of NMI_occurred in bit 7, then set
    // NMI_occurred to false.
    *ppu_status = clear_bit(*ppu_status, VBLANK);
}

void PPU::ppu_scroll_write()
{
    if (!write_toggle) { // First write.
        *ppu_scroll = (tmp_addr << 3) | finex_scroll;
    } else {    // Second write.
        *ppu_scroll = (uint8_t) ((tmp_addr >> 2) | (tmp_addr >> 12));
    }
    write_toggle = !write_toggle;
}

void PPU::ppu_addr_write()
{
    if (!write_toggle) { // First write.
        *ppu_addr = high_byte(tmp_addr);
        *ppu_addr = set_bit(*ppu_addr, bit_pos(14));
    }
    else {    // Second write.
        *ppu_addr = low_byte(tmp_addr);
    }
    write_toggle = !write_toggle;
}

void PPU::rendering()
{
    // TODO: Fetch bit from 4 background shift registers.
    bk_16shf_reg[0] >>= 1;
    bk_16shf_reg[1] >>= 1;
    bk_8shf_reg[0] >>= 1;
    bk_8shf_reg[1] >>= 1;
    // TODO: Every 8 cycles/shifts, load new data into these registers.
    
}

}   // Namespace ppu.