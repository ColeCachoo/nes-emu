// addressing.cpp : Defines the addressing mode instructions used by the CPU
// instructions.
//
#include "cpu.h"

uint16_t 
CPU::immediate() {
    return fetch();
}

uint16_t 
CPU::zero_page() {
    return fetch();
}

uint16_t 
CPU::zero_page_x() {
    return (x_index + fetch()) % 256;
}

uint16_t 
CPU::zero_page_y() {
    return (y_index + fetch()) % 256;
}

int8_t 
CPU::relative() {
    return int8_t(fetch());
}

uint16_t 
CPU::absolute() {
    uint16_t addr_low  = fetch();
    uint16_t addr_high = fetch();
    addr_high <<= 8;
    return (addr_high | addr_low);
}

uint16_t 
CPU::absolute_x() {
    uint16_t addr_low  = fetch();
    uint16_t addr_high = fetch();
    addr_high <<= 8;
    return (addr_high | addr_low) + x_index;
}

uint16_t 
CPU::absolute_y() {
    uint16_t addr_low  = fetch();
    uint16_t addr_high = fetch();
    addr_high <<= 8;
    return (addr_high | addr_low) + y_index;
}

uint16_t 
CPU::indirect() {
    uint16_t addr_low  = fetch();
    uint16_t addr_high = fetch();
    addr_high <<= 8;
    uint16_t new_low  = ram->read(addr_high | addr_low);
    // An original 6502 has does not correctly fetch the target address if the
    // indirect vector falls on a page boundary (e.g. $xxFF where xx is any value
    // from $00 to $FF). In this case fetches the LSB from $xxFF as expected but
    // takes the MSB from $xx00.
    uint16_t new_high = ram->read(addr_high | uint8_t(addr_low + 1));
    // This would fix the bug, left in case I need it.
    // uint16_t new_high = ram->read((addr_high | addr_low) + 1);
    new_high <<= 8;
    return (new_high | new_low);
}

uint16_t 
CPU::indexed_indirect() {
    uint8_t val = fetch();
    return ram->read((val + x_index) % 256) + ram->read((val + x_index + 1) % 256) * 256;

}

uint16_t 
CPU::indirect_indexed() {
    uint8_t val = fetch();
    return ram->read(val) + ram->read((val + 1) % 256) * 256 + y_index;
}
