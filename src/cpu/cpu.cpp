// cpu.cpp : File contains class specific functions (like class constructor)
// and helper functions.
//
#include "cpu/cpu.h"
#include "error.h"

#include <fmt/format.h>
#include <fmt/ostream.h>

#include <fstream>
#include <iostream>

using namespace std;

CPU::CPU(RAM *r) {
    program_counter = 0xc000;
    stack_pointer   = 0xfd;
    accumulator     = 0x00;
    x_index         = 0x00;
    y_index         = 0x00;
    cpu_status      = 0x24;

    this->ram = r;

    // TODO: This should get the starting address of the ROM.
    uint16_t start_pcl = ram->read(0xfffc);
    uint16_t start_pch = ram->read(0xfffd) << 8;
    program_counter = (start_pch | start_pcl);
    fmt::print("Program Counter: {:X}\n", program_counter);
    program_counter = 0xc000;
    fmt::print("Program Counter: {:X}\n", program_counter);

    ofstream my_log("../resources/my_log.txt", ofstream::out | ofstream::trunc);
    if (!my_log.is_open()) {
        fmt::print("Failed to open my_log.txt\n");
    }

    // Run.
    current_op = 0;
    for (size_t i = 0; i < 8991; i++) {
        uint8_t opcode = fetch();
        current_op = opcode;
        println(my_log);
        if (decode(opcode) == ERROR) {
            fmt::print("Unrecognized opcode: 0x{:X}\n", opcode);
            break;
        }
    }

    my_log.close();
}

void 
CPU::stack_push(const uint8_t val) {
    ram->write(val, STACK_BASE + stack_pointer);
    stack_pointer--;
}

uint8_t 
CPU::stack_pop() {
    stack_pointer++;
    return ram->read(STACK_BASE + stack_pointer);
}

void 
CPU::set_zero_if(const uint8_t val) {
    if (val == 0) {
        cpu_status.set(Zero);
    } else {
        cpu_status.reset(Zero);
    }
}

void 
CPU::set_negative_if(const uint8_t val) {
    if (val & 0x80) {
        cpu_status.set(Negative);
    } else {
        cpu_status.reset(Negative);
    }
}

uint8_t 
CPU::fetch() {
    return ram->read(program_counter++);
}

uint16_t 
CPU::fetch_with(Addressing addr_mode) {
    uint16_t addr = 0;
    switch (addr_mode) {
    case Addressing::Immediate:
        addr = immediate();
        break;
    case Addressing::ZeroPage:
        addr = zero_page();
        break;
    case Addressing::ZeroPageX:
        addr = zero_page_x();
        break;
    case Addressing::ZeroPageY:
        addr = zero_page_y();
        break;
    case Addressing::Relative:
        addr = relative();
        break;
    case Addressing::Absolute:
        addr = absolute();
        break;
    case Addressing::AbsoluteX:
        addr = absolute_x();
        break;
    case Addressing::AbsoluteY:
        addr = absolute_y();
        break;
    case Addressing::Indirect:
        addr = indirect();
        break;
    case Addressing::IndexedIndirect:
        addr = indexed_indirect();
        break;
    case Addressing::IndirectIndexed:
        addr = indirect_indexed();
        break;
    default :
        fmt::print(stderr, "Unexpected addressing mode in fetch_with(): {}\n", int(addr_mode));
    }

    return addr;
}

// TODO: Instead of this, add the >> operator to class.
void
CPU::println() const {
    fmt::print("{:04X} {:02X} A:{:02X} X:{:02X} Y:{:02X} P:{:02X} SP:{:02X}\n",
        program_counter - 1,
        current_op,
        accumulator,
        x_index,
        y_index,
        cpu_status.to_ulong(),
        stack_pointer
    );
}

void
CPU::println(ofstream &my_log) const {
    fmt::print(my_log, "{:04X} {:02X} A:{:02X} X:{:02X} Y:{:02X} P:{:02X} SP:{:02X}\n",
        program_counter - 1,
        current_op,
        accumulator,
        x_index,
        y_index,
        cpu_status.to_ulong(),
        stack_pointer
    );
}

error_t 
CPU::decode(const uint8_t opcode) {
    switch (opcode) {
    case 0xa9: lda(uint8_t(fetch_with(Addressing::Immediate)));                     break;
    case 0xa5: lda(ram->read(fetch_with(Addressing::ZeroPage)));                    break;
    case 0xb5: lda(ram->read(fetch_with(Addressing::ZeroPageX)));                   break;
    case 0xad: lda(ram->read(fetch_with(Addressing::Absolute)));                    break;
    case 0xbd: lda(ram->read(fetch_with(Addressing::AbsoluteX)));                   break;
    case 0xb9: lda(ram->read(fetch_with(Addressing::AbsoluteY)));                   break;
    case 0xa1: lda(ram->read(fetch_with(Addressing::IndexedIndirect)));             break;
    case 0xb1: lda(ram->read(fetch_with(Addressing::IndirectIndexed)));             break;
    case 0xa2: ldx(uint8_t(fetch_with(Addressing::Immediate)));                     break;
    case 0xa6: ldx(ram->read(fetch_with(Addressing::ZeroPage)));                    break;
    case 0xb6: ldx(ram->read(fetch_with(Addressing::ZeroPageY)));                   break;
    case 0xae: ldx(ram->read(fetch_with(Addressing::Absolute)));                    break;
    case 0xbe: ldx(ram->read(fetch_with(Addressing::AbsoluteY)));                   break;
    case 0xa0: ldy(uint8_t(fetch_with(Addressing::Immediate)));                     break;
    case 0xa4: ldy(ram->read(fetch_with(Addressing::ZeroPage)));                    break;
    case 0xb4: ldy(ram->read(fetch_with(Addressing::ZeroPageX)));                   break;
    case 0xac: ldy(ram->read(fetch_with(Addressing::Absolute)));                    break;
    case 0xbc: ldy(ram->read(fetch_with(Addressing::AbsoluteX)));                   break;
    case 0x85: sta(fetch_with(Addressing::ZeroPage));                               break;
    case 0x95: sta(fetch_with(Addressing::ZeroPageX));                              break;
    case 0x8d: sta(fetch_with(Addressing::Absolute));                               break;
    case 0x9d: sta(fetch_with(Addressing::AbsoluteX));                              break;
    case 0x99: sta(fetch_with(Addressing::AbsoluteY));                              break;
    case 0x81: sta(fetch_with(Addressing::IndexedIndirect));                        break;
    case 0x91: sta(fetch_with(Addressing::IndirectIndexed));                        break;
    case 0x86: stx(fetch_with(Addressing::ZeroPage));                               break;
    case 0x96: stx(fetch_with(Addressing::ZeroPageY));                              break;
    case 0x8e: stx(fetch_with(Addressing::Absolute));                               break;
    case 0x84: sty(fetch_with(Addressing::ZeroPage));                               break;
    case 0x94: sty(fetch_with(Addressing::ZeroPageX));                              break;
    case 0x8c: sty(fetch_with(Addressing::Absolute));                               break;
    case 0xaa: tax();                                                               break;
    case 0xa8: tay();                                                               break;
    case 0x8a: txa();                                                               break;
    case 0x98: tya();                                                               break;
    case 0xba: tsx();                                                               break;
    case 0x9a: txs();                                                               break;
    case 0x48: pha();                                                               break;
    case 0x08: php();                                                               break;
    case 0x68: pla();                                                               break;
    case 0x28: plp();                                                               break;
    case 0x29: logical_and(uint8_t(fetch_with(Addressing::Immediate)));             break;
    case 0x25: logical_and(ram->read(fetch_with(Addressing::ZeroPage)));            break;
    case 0x35: logical_and(ram->read(fetch_with(Addressing::ZeroPageX)));           break;
    case 0x2d: logical_and(ram->read(fetch_with(Addressing::Absolute)));            break;
    case 0x3d: logical_and(ram->read(fetch_with(Addressing::AbsoluteX)));           break;
    case 0x39: logical_and(ram->read(fetch_with(Addressing::AbsoluteY)));           break;
    case 0x21: logical_and(ram->read(fetch_with(Addressing::IndexedIndirect)));     break;
    case 0x31: logical_and(ram->read(fetch_with(Addressing::IndirectIndexed)));     break;
    case 0x49: eor(uint8_t(fetch_with(Addressing::Immediate)));                     break;
    case 0x45: eor(ram->read(fetch_with(Addressing::ZeroPage)));                    break;
    case 0x55: eor(ram->read(fetch_with(Addressing::ZeroPageX)));                   break;
    case 0x4d: eor(ram->read(fetch_with(Addressing::Absolute)));                    break;
    case 0x5d: eor(ram->read(fetch_with(Addressing::AbsoluteX)));                   break;
    case 0x59: eor(ram->read(fetch_with(Addressing::AbsoluteY)));                   break;
    case 0x41: eor(ram->read(fetch_with(Addressing::IndexedIndirect)));             break;
    case 0x51: eor(ram->read(fetch_with(Addressing::IndirectIndexed)));             break;
    case 0x09: ora(uint8_t(fetch_with(Addressing::Immediate)));                     break;
    case 0x05: ora(ram->read(fetch_with(Addressing::ZeroPage)));                    break;
    case 0x15: ora(ram->read(fetch_with(Addressing::ZeroPageX)));                   break;
    case 0x0d: ora(ram->read(fetch_with(Addressing::Absolute)));                    break;
    case 0x1d: ora(ram->read(fetch_with(Addressing::AbsoluteX)));                   break;
    case 0x19: ora(ram->read(fetch_with(Addressing::AbsoluteY)));                   break;
    case 0x01: ora(ram->read(fetch_with(Addressing::IndexedIndirect)));             break;
    case 0x11: ora(ram->read(fetch_with(Addressing::IndirectIndexed)));             break;
    case 0x24: bit(ram->read(fetch_with(Addressing::ZeroPage)));                    break;
    case 0x2c: bit(ram->read(fetch_with(Addressing::Absolute)));                    break;
    case 0x69: adc(uint8_t(fetch_with(Addressing::Immediate)));                     break;
    case 0x65: adc(ram->read(fetch_with(Addressing::ZeroPage)));                    break;
    case 0x75: adc(ram->read(fetch_with(Addressing::ZeroPageX)));                   break;
    case 0x6d: adc(ram->read(fetch_with(Addressing::Absolute)));                    break;
    case 0x7d: adc(ram->read(fetch_with(Addressing::AbsoluteX)));                   break;
    case 0x79: adc(ram->read(fetch_with(Addressing::AbsoluteY)));                   break;
    case 0x61: adc(ram->read(fetch_with(Addressing::IndexedIndirect)));             break;
    case 0x71: adc(ram->read(fetch_with(Addressing::IndirectIndexed)));             break;
    case 0xe9: sbc(uint8_t(fetch_with(Addressing::Immediate)));                     break;
    case 0xe5: sbc(ram->read(fetch_with(Addressing::ZeroPage)));                    break;
    case 0xf5: sbc(ram->read(fetch_with(Addressing::ZeroPageX)));                   break;
    case 0xed: sbc(ram->read(fetch_with(Addressing::Absolute)));                    break;
    case 0xfd: sbc(ram->read(fetch_with(Addressing::AbsoluteX)));                   break;
    case 0xf9: sbc(ram->read(fetch_with(Addressing::AbsoluteY)));                   break;
    case 0xe1: sbc(ram->read(fetch_with(Addressing::IndexedIndirect)));             break;
    case 0xf1: sbc(ram->read(fetch_with(Addressing::IndirectIndexed)));             break;
    case 0xc9: cmp(uint8_t(fetch_with(Addressing::Immediate)));                     break;
    case 0xc5: cmp(ram->read(fetch_with(Addressing::ZeroPage)));                    break;
    case 0xd5: cmp(ram->read(fetch_with(Addressing::ZeroPageX)));                   break;
    case 0xcd: cmp(ram->read(fetch_with(Addressing::Absolute)));                    break;
    case 0xdd: cmp(ram->read(fetch_with(Addressing::AbsoluteX)));                   break;
    case 0xd9: cmp(ram->read(fetch_with(Addressing::AbsoluteY)));                   break;
    case 0xc1: cmp(ram->read(fetch_with(Addressing::IndexedIndirect)));             break;
    case 0xd1: cmp(ram->read(fetch_with(Addressing::IndirectIndexed)));             break;
    case 0xe0: cpx(uint8_t(fetch_with(Addressing::Immediate)));                     break;
    case 0xe4: cpx(ram->read(fetch_with(Addressing::ZeroPage)));                    break;
    case 0xec: cpx(ram->read(fetch_with(Addressing::Absolute)));                    break;
    case 0xc0: cpy(uint8_t(fetch_with(Addressing::Immediate)));                     break;
    case 0xc4: cpy(ram->read(fetch_with(Addressing::ZeroPage)));                    break;
    case 0xcc: cpy(ram->read(fetch_with(Addressing::Absolute)));                    break;
    case 0xe6: inc(fetch_with(Addressing::ZeroPage));                               break;
    case 0xf6: inc(fetch_with(Addressing::ZeroPageX));                              break;
    case 0xee: inc(fetch_with(Addressing::Absolute));                               break;
    case 0xfe: inc(fetch_with(Addressing::AbsoluteX));                              break;
    case 0xe8: inx();                                                               break;
    case 0xc8: iny();                                                               break;
    case 0xc6: dec(fetch_with(Addressing::ZeroPage));                               break;
    case 0xd6: dec(fetch_with(Addressing::ZeroPageX));                              break;
    case 0xce: dec(fetch_with(Addressing::Absolute));                               break;
    case 0xde: dec(fetch_with(Addressing::AbsoluteX));                              break;
    case 0xca: dex();                                                               break;
    case 0x88: dey();                                                               break;
    case 0x0a: asl(Addressing::Accumulator);                                        break;
    case 0x06: asl(Addressing::ZeroPage);                                           break;
    case 0x16: asl(Addressing::ZeroPageX);                                          break;
    case 0x0e: asl(Addressing::Absolute);                                           break;
    case 0x1e: asl(Addressing::AbsoluteX);                                          break;
    case 0x4a: lsr(Addressing::Accumulator);                                        break;
    case 0x46: lsr(Addressing::ZeroPage);                                           break;
    case 0x56: lsr(Addressing::ZeroPageX);                                          break;
    case 0x4e: lsr(Addressing::Absolute);                                           break;
    case 0x5e: lsr(Addressing::AbsoluteX);                                          break;
    case 0x2a: rol(Addressing::Accumulator);                                        break;
    case 0x26: rol(Addressing::ZeroPage);                                           break;
    case 0x36: rol(Addressing::ZeroPageX);                                          break;
    case 0x2e: rol(Addressing::Absolute);                                           break;
    case 0x3e: rol(Addressing::AbsoluteX);                                          break;
    case 0x6a: ror(Addressing::Accumulator);                                        break;
    case 0x66: ror(Addressing::ZeroPage);                                           break;
    case 0x76: ror(Addressing::ZeroPageX);                                          break;
    case 0x6e: ror(Addressing::Absolute);                                           break;
    case 0x7e: ror(Addressing::AbsoluteX);                                          break;
    case 0x4c: jmp(fetch_with(Addressing::Absolute));                               break;
    case 0x6c: jmp(fetch_with(Addressing::Indirect));                               break;
    case 0x20: jsr(fetch_with(Addressing::Absolute));                               break;
    case 0x60: rts();                                                               break;
    case 0x90: bcc();                                                               break;
    case 0xb0: bcs();                                                               break;
    case 0xf0: beq();                                                               break;
    case 0x30: bmi();                                                               break;
    case 0xd0: bne();                                                               break;
    case 0x10: bpl();                                                               break;
    case 0x50: bvc();                                                               break;
    case 0x70: bvs();                                                               break;
    case 0x18: clc();                                                               break;
    case 0xd8: cld();                                                               break;
    case 0x58: cli();                                                               break;
    case 0xb8: clv();                                                               break;
    case 0x38: sec();                                                               break;
    case 0xf8: sed();                                                               break;
    case 0x78: sei();                                                               break;
    case 0x00: brk();                                                               break;
    case 0xea: nop();                                                               break;
    case 0x40: rti();                                                               break;
    // Below here are unofficial opcodes.
    case 0x0c: fetch_with(Addressing::Absolute);  nop();                            break;
    case 0x04: fetch_with(Addressing::ZeroPage);  nop();                            break;
    case 0x14: fetch_with(Addressing::ZeroPageX); nop();                            break;
    case 0x1c: fetch_with(Addressing::AbsoluteX); nop();                            break;
    case 0x1a: nop();                                                               break;
    case 0x34: fetch_with(Addressing::ZeroPageX); nop();                            break;
    case 0x3c: fetch_with(Addressing::AbsoluteX); nop();                            break;
    case 0x3a: nop();                                                               break;
    case 0x44: fetch_with(Addressing::ZeroPage);  nop();                            break;
    case 0x54: fetch_with(Addressing::ZeroPageX); nop();                            break;
    case 0x5c: fetch_with(Addressing::AbsoluteX); nop();                            break;
    case 0x5a: nop();                                                               break;
    case 0x64: fetch_with(Addressing::ZeroPage);  nop();                            break;
    case 0x74: fetch_with(Addressing::ZeroPageX); nop();                            break;
    case 0x7c: fetch_with(Addressing::AbsoluteX); nop();                            break;
    case 0x7a: nop();                                                               break;
    case 0x80: fetch_with(Addressing::Immediate); nop();                            break;
    case 0x89: fetch_with(Addressing::Immediate); nop();                            break;
    case 0x82: fetch_with(Addressing::Immediate); nop();                            break;
    case 0xd4: fetch_with(Addressing::ZeroPageX); nop();                            break;
    case 0xdc: fetch_with(Addressing::AbsoluteX); nop();                            break;
    case 0xc2: fetch_with(Addressing::Immediate); nop();                            break;
    case 0xda: nop();                                                               break;
    case 0xf4: fetch_with(Addressing::ZeroPageX); nop();                            break;
    case 0xfc: fetch_with(Addressing::AbsoluteX); nop();                            break;
    case 0xe2: fetch_with(Addressing::Immediate); nop();                            break;
    case 0xfa: nop();                                                               break;
    default:
        return ERROR;
    }
    return SUCCESS;
}