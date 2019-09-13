// nes-error.h : nes-emu error codes.
//
#pragma once

enum class NesError {
    Success,
    Err,
    BadAlloc,
    CouldNotOpenFile,
    InvalidOpcode,
};