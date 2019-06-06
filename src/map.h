// map.h : Reads data from ROM and maps it to RAM.
//
#pragma once

#include "ram.h"

#include <string>

/// Maps the rom to RAM.
/// Returns true if error occurred, else returns false.
bool map(const std::string &rom_path, RAM &ram);