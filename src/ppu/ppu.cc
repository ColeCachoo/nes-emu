// ppu.cc
//
#include "ppu/ppu.h"
#include "nes-utils.h"

// Detailed comments taken from:
// http://wiki.nesdev.com/w/index.php/PPU_programmer_reference

namespace ppu {

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
        *ppu_scroll = uint8_t(((tmp_addr >> 2) | (tmp_addr >> 12)));
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