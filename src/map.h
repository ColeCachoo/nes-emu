// map.h : Reads data from ROM and maps it to RAM/VRAM.
//
#pragma once

#include "nes-error.h"

#include <cstdint>
#include <string>

/// Maps the rom to RAM and VRAM.
/// Returns true if error occurred, else returns false.
NesError map(const std::string &rom_path, uint8_t *ram, uint8_t *vram);