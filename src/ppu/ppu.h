// ppu.h
//
#pragma once

#include "nes-error.h"

#include <cstdint>
#include <memory>

namespace ppu {

class PPU {
public:
    PPU(uint8_t ram[], uint8_t vram[]);
    ~PPU() = default;

    void fetch();
    void run();
    void ppu_scroll_write();
    void ppu_addr_write();
    void rendering();

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
    uint8_t *vram;

    // 256 bytes OAM.
    std::unique_ptr<uint8_t[]> oam;

    // TODO: Better names, please. Are these even used?
    // Background.
    uint16_t bk_16shf_reg[2];
    uint8_t bk_8shf_reg[2];
    // Sprites.
    uint16_t spr_16shf_reg[8];
    uint8_t spr_8shf_reg[2];
    uint8_t latches[8];
    uint8_t counters[8];

    // Flags for PPUCTRL.
    static constexpr uint8_t NAMETABLE_0         = 1 << 0;   // (N) Nametable select (bit position 0).
    static constexpr uint8_t NAMETABLE_1         = 1 << 1;   // (N) Nametable select (bit position 1).
    static constexpr uint8_t INCREMENT           = 1 << 2;   // (I) Increment mode.
    static constexpr uint8_t SPRITE_TILE         = 1 << 3;   // (S) Sprite tile select.
    static constexpr uint8_t BACKGROUND_TILE     = 1 << 4;   // (B) Background tile select.
    static constexpr uint8_t SPRITE_SIZE         = 1 << 5;   // (H) Sprite size.
    static constexpr uint8_t PPU_MASTER_SLAVE    = 1 << 6;   // (P) PPU master/slave.
    static constexpr uint8_t NMI_ENABLE          = 1 << 7;   // (V) NMI Enable.

    // Flags for PPUMASK.
    static constexpr uint8_t GREYSCALE           = 1 << 0;   // (G) Greyscale.
    static constexpr uint8_t BACKGROUND_LEFT     = 1 << 1;   // (m) Background left column enable.
    static constexpr uint8_t SPRITE_LEFT         = 1 << 2;   // (M) Sprite left column enable.
    static constexpr uint8_t BACKGROUND_ENABLE   = 1 << 3;   // (b) Background enable.
    static constexpr uint8_t SPRITE_ENABLE       = 1 << 4;   // (s) Sprite enable.
    static constexpr uint8_t COLOR_R             = 1 << 5;   // (R) Color emphasis (red).
    static constexpr uint8_t COLOR_G             = 1 << 6;   // (G) Color emphasis (green).
    static constexpr uint8_t COLOR_B             = 1 << 7;   // (B) Color emphasis (blue).

    // Flags for PPUSTATUS.
    static constexpr uint8_t SPRITE_OVERFLOW     = 1 << 5;   // (O) Sprite overflow.
    static constexpr uint8_t SPRITE_HIT          = 1 << 6;   // (S) Sprite 0 hit.
    static constexpr uint8_t VBLANK              = 1 << 7;   // (V) vblank.

    int current_scanline;
};

}   // Namespace ppu.