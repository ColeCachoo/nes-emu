// ppu.hpp
//
#pragma once

#include "neserror.hpp"

#include <cstdint>
#include <memory>

namespace ppu {

class PPU {
public:
    PPU(uint8_t *ram);
    ~PPU() = default;

    void fetch();
    void run();
    void ppu_scroll_write();
    void ppu_addr_write();
    void per_cycle();

private:
    // The 15 bit registers current and tmp are composed this way during
    // rendering:
    /**************************************************
            yyy NN YYYYY XXXXX
            ||| || ||||| +++++-- coarse X scroll
            ||| || +++++-------- coarse Y scroll
            ||| ++-------------- nametable select
            +++----------------- fine Y scroll
    ***************************************************/
    // Internal registers.
    uint16_t current_addr;  // (v) Current VRAM address (15 bits).
    uint16_t tmp_addr;      // (t) Temporary VRAM address (15 bits).
    uint8_t finex_scroll;   // (x) Fine X scroll (3 bits).
    bool write_toggle;      // (w) First or second write toggle (1 bit).

    // Memory-mapped registers.
    //
    //        Name           |       Bits       |      Description
    uint8_t *ppu_ctrl;   //  |     VPHB SINN    |  See flags in enum PPUCtrl.
    uint8_t *ppu_mask;   //  |     BGRs bMmG    |  See flags in enum PPUMask.
    uint8_t *ppu_status; //  |     VSO- ----    |  See flags in enum PPUStatus.
    uint8_t *oam_addr;   //  |     aaaa aaaa    |  OAM read/write address.
    uint8_t *oam_data;   //  |     dddd dddd    |  OAM data read/write.
    uint8_t *ppu_scroll; //  |     xxxx xxxx    |  Fine scroll position (two writes: X scroll, 
                         //  |                  |  Y scroll).
    uint8_t *ppu_addr;   //  |     aaaa aaaa    |  PPU read/write address (two writes: most 
                         //  |                  |  significant byte, least significant byte).
    uint8_t *ppu_data;   //  |     dddd dddd    |  PPU data read/write.
    uint8_t *oam_dma;    //  |     aaaa aaaa    |  OAM DMA high address.

    // 16kB VRAM.
    std::unique_ptr<uint8_t[]> vram;

    // 256 bytes OAM.
    std::unique_ptr<uint8_t[]> oam;

    // uint8_t latch;
};

}  // namespace ppu