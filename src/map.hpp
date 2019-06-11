// map.hpp : Reads data from ROM and maps it to RAM/VRAM.
//
#pragma once

#include "neserror.hpp"

#include <cstdint>

/// Maps the rom to RAM.
/// Returns true if error occurred, else returns false.
neserror_t map(const char *rom_path, uint8_t *ram);