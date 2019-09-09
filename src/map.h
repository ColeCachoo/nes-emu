// map.h : Reads data from ROM and maps it to RAM/VRAM.
//
#pragma once

#include "nes-error.h"

#include <cstdint>

/// Maps the rom to RAM.
/// Returns true if error occurred, else returns false.
NESerror map(const char *rom_path, uint8_t *ram);