// cpu.cc : Contains public functions.
//
#include "cpu.h"
#include "nes-utils.h"

#include <fmt/format.h>
#include <fmt/ostream.h>

namespace cpu {

CPU::CPU(uint8_t *mem)
{
    // TODO: This should get the starting address of the ROM. I'm not sure if it
    // works or not.
    program_counter = (mem[0xfffc]) | (mem[0xfffd] << 8);
    stack_pointer   = 0xfd;
    accumulator     = 0x00;
    x_index         = 0x00;
    y_index         = 0x00;
    status          = 0x24;
    ram             = mem;

    fmt::print("Program Counter: {:X}\n", program_counter);
    program_counter = 0xc000;
    fmt::print("Program Counter: {:X}\n", program_counter);
}

void CPU::print() const
{
    fmt::print("{:04X} {:02X} A:{:02X} X:{:02X} Y:{:02X} P:{:02X} SP:{:02X}\n",
        program_counter - 1,
        ram[program_counter - 1],     // Current opcode.
        accumulator,
        x_index,
        y_index,
        status,
        stack_pointer
    );
}

void CPU::fprint(std::ofstream &file) const
{
    fmt::print(file, "{:04X} {:02X} A:{:02X} X:{:02X} Y:{:02X} P:{:02X} SP:{:02X}\n",
        program_counter - 1,
        ram[program_counter - 1],     // Current opcode.
        accumulator,
        x_index,
        y_index,
        status,
        stack_pointer
    );
}

// TODO: Delete this. All this should be done by the calling function.
NESerror CPU::run()
{
    // FILE *my_log = fopen("E:/Documents/programming/c-lang/test/resources/my_log.txt", "w");
    std::ofstream my_log("../resources/my_log.txt", std::ofstream::out | std::ofstream::trunc);
    if (!my_log.is_open()) {
        fmt::print(stderr, "Failed to open my_log.txt\n");
        return ERROR;
    }

    // Run.
    for (size_t i = 0; i < 8991; i++) {
        const uint8_t opcode = fetch();
        fprint(my_log);
        if (execute(opcode) == ERROR) {
            fmt::print(stderr, "Unrecognized opcode: 0x{:X}\n", opcode);
            break;
        }
    }

    my_log.close();
    return SUCCESS;
}

// TODO: Think of good name for the argument.
void CPU::interrupt(Interrupt arg)
{
    // TODO: Is this how much I should increment PC?
    program_counter += 2;
    stack_push(high_byte(program_counter));
    stack_push(low_byte(program_counter));
    status = set_bit(status, BREAK | EXPANSION);
    stack_push(status);
    status = set_bit(status, INTERRUPT);

    uint16_t addr_low = 0;
    uint16_t addr_high = 0;
    if (arg == IRQ || arg == BRK) {
        addr_low  = 0xfffe;
        addr_high = 0xffff;
    } else {    // NMI/RESET
        addr_low  = 0xfffa;
        addr_high = 0xfffb;
    }

    const uint16_t vec_low  = ((uint16_t) ram[addr_low]);
    const uint16_t vec_high = ((uint16_t) ram[addr_high]) << 8;
    program_counter = (vec_high | vec_low);
}

NESerror CPU::execute(uint8_t opcode)
{
    switch (opcode) {
    case 0xa9: lda(immediate()); break;
    case 0xa5: lda(zero_page()); break;
    case 0xb5: lda(zero_page_x()); break;
    case 0xad: lda(absolute()); break;
    case 0xbd: lda(absolute_x()); break;
    case 0xb9: lda(absolute_y()); break;
    case 0xa1: lda(indexed_indirect()); break;
    case 0xb1: lda(indirect_indexed()); break;
    case 0xa2: ldx(immediate()); break;
    case 0xa6: ldx(zero_page()); break;
    case 0xb6: ldx(zero_page_y()); break;
    case 0xae: ldx(absolute()); break;
    case 0xbe: ldx(absolute_y()); break;
    case 0xa0: ldy(immediate()); break;
    case 0xa4: ldy(zero_page()); break;
    case 0xb4: ldy(zero_page_x()); break;
    case 0xac: ldy(absolute()); break;
    case 0xbc: ldy(absolute_x()); break;
    case 0x85: sta(zero_page_addr()); break;
    case 0x95: sta(zero_page_x_addr()); break;
    case 0x8d: sta(absolute_addr()); break;
    case 0x9d: sta(absolute_x_addr()); break;
    case 0x99: sta(absolute_y_addr()); break;
    case 0x81: sta(indexed_indirect_addr()); break;
    case 0x91: sta(indirect_indexed_addr()); break;
    case 0x86: stx(zero_page_addr()); break;
    case 0x96: stx(zero_page_y_addr()); break;
    case 0x8e: stx(absolute_addr()); break;
    case 0x84: sty(zero_page_addr()); break;
    case 0x94: sty(zero_page_x_addr()); break;
    case 0x8c: sty(absolute_addr()); break;
    case 0xaa: tax(); break;
    case 0xa8: tay(); break;
    case 0x8a: txa(); break;
    case 0x98: tya(); break;
    case 0xba: tsx(); break;
    case 0x9a: txs(); break;
    case 0x48: pha(); break;
    case 0x08: php(); break;
    case 0x68: pla(); break;
    case 0x28: plp(); break;
    case 0x29: logical_and(immediate()); break;
    case 0x25: logical_and(zero_page()); break;
    case 0x35: logical_and(zero_page_x()); break;
    case 0x2d: logical_and(absolute()); break;
    case 0x3d: logical_and(absolute_x()); break;
    case 0x39: logical_and(absolute_y()); break;
    case 0x21: logical_and(indexed_indirect()); break;
    case 0x31: logical_and(indirect_indexed()); break;
    case 0x49: eor(immediate()); break;
    case 0x45: eor(zero_page()); break;
    case 0x55: eor(zero_page_x()); break;
    case 0x4d: eor(absolute()); break;
    case 0x5d: eor(absolute_x()); break;
    case 0x59: eor(absolute_y()); break;
    case 0x41: eor(indexed_indirect()); break;
    case 0x51: eor(indirect_indexed()); break;
    case 0x09: ora(immediate()); break;
    case 0x05: ora(zero_page()); break;
    case 0x15: ora(zero_page_x()); break;
    case 0x0d: ora(absolute()); break;
    case 0x1d: ora(absolute_x()); break;
    case 0x19: ora(absolute_y()); break;
    case 0x01: ora(indexed_indirect()); break;
    case 0x11: ora(indirect_indexed()); break;
    case 0x24: bit(zero_page()); break;
    case 0x2c: bit(absolute()); break;
    case 0x69: adc(immediate()); break;
    case 0x65: adc(zero_page()); break;
    case 0x75: adc(zero_page_x()); break;
    case 0x6d: adc(absolute()); break;
    case 0x7d: adc(absolute_x()); break;
    case 0x79: adc(absolute_y()); break;
    case 0x61: adc(indexed_indirect()); break;
    case 0x71: adc(indirect_indexed()); break;
    case 0xe9: sbc(immediate()); break;
    case 0xe5: sbc(zero_page()); break;
    case 0xf5: sbc(zero_page_x()); break;
    case 0xed: sbc(absolute()); break;
    case 0xfd: sbc(absolute_x()); break;
    case 0xf9: sbc(absolute_y()); break;
    case 0xe1: sbc(indexed_indirect()); break;
    case 0xf1: sbc(indirect_indexed()); break;
    case 0xc9: cmp(immediate()); break;
    case 0xc5: cmp(zero_page()); break;
    case 0xd5: cmp(zero_page_x()); break;
    case 0xcd: cmp(absolute()); break;
    case 0xdd: cmp(absolute_x()); break;
    case 0xd9: cmp(absolute_y()); break;
    case 0xc1: cmp(indexed_indirect()); break;
    case 0xd1: cmp(indirect_indexed()); break;
    case 0xe0: cpx(immediate()); break;
    case 0xe4: cpx(zero_page()); break;
    case 0xec: cpx(absolute()); break;
    case 0xc0: cpy(immediate()); break;
    case 0xc4: cpy(zero_page()); break;
    case 0xcc: cpy(absolute()); break;
    case 0xe6: inc(zero_page_addr()); break;
    case 0xf6: inc(zero_page_x_addr()); break;
    case 0xee: inc(absolute_addr()); break;
    case 0xfe: inc(absolute_x_addr()); break;
    case 0xe8: inx(); break;
    case 0xc8: iny(); break;
    case 0xc6: dec(zero_page_addr()); break;
    case 0xd6: dec(zero_page_x_addr()); break;
    case 0xce: dec(absolute_addr()); break;
    case 0xde: dec(absolute_x_addr()); break;
    case 0xca: dex(); break;
    case 0x88: dey(); break;
    case 0x0a: asl(get_accumulator()); break;
    case 0x06: asl(&ram[zero_page_addr()]); break;
    case 0x16: asl(&ram[zero_page_x_addr()]); break;
    case 0x0e: asl(&ram[absolute_addr()]); break;
    case 0x1e: asl(&ram[absolute_x_addr()]); break;
    case 0x4a: lsr(get_accumulator()); break;
    case 0x46: lsr(&ram[zero_page_addr()]); break;
    case 0x56: lsr(&ram[zero_page_x_addr()]); break;
    case 0x4e: lsr(&ram[absolute_addr()]); break;
    case 0x5e: lsr(&ram[absolute_x_addr()]); break;
    case 0x2a: rol(get_accumulator()); break;
    case 0x26: rol(&ram[zero_page_addr()]); break;
    case 0x36: rol(&ram[zero_page_x_addr()]); break;
    case 0x2e: rol(&ram[absolute_addr()]); break;
    case 0x3e: rol(&ram[absolute_x_addr()]); break;
    case 0x6a: ror(get_accumulator()); break;
    case 0x66: ror(&ram[zero_page_addr()]); break;
    case 0x76: ror(&ram[zero_page_x_addr()]); break;
    case 0x6e: ror(&ram[absolute_addr()]); break;
    case 0x7e: ror(&ram[absolute_x_addr()]); break;
    case 0x4c: jmp(absolute_addr()); break;
    case 0x6c: jmp(indirect_addr()); break;
    case 0x20: jsr(absolute_addr()); break;
    case 0x60: rts(); break;
    case 0x90: bcc(); break;
    case 0xb0: bcs(); break;
    case 0xf0: beq(); break;
    case 0x30: bmi(); break;
    case 0xd0: bne(); break;
    case 0x10: bpl(); break;
    case 0x50: bvc(); break;
    case 0x70: bvs(); break;
    case 0x18: clc(); break;
    case 0xd8: cld(); break;
    case 0x58: cli(); break;
    case 0xb8: clv(); break;
    case 0x38: sec(); break;
    case 0xf8: sed(); break;
    case 0x78: sei(); break;
    case 0x00: brk(); break;
    case 0xea: nop(); break;
    case 0x40: rti(); break;
    // Below here are unofficial opcodes.
    case 0x0c: absolute();       nop(); break;
    case 0x04: zero_page();      nop(); break;
    case 0x14: zero_page_x();    nop(); break;
    case 0x1c: absolute_x();     nop(); break;
    case 0x1a: nop(); break;
    case 0x34: zero_page_x();    nop(); break;
    case 0x3c: absolute_x();     nop(); break;
    case 0x3a: nop(); break;
    case 0x44: zero_page();      nop(); break;
    case 0x54: zero_page_x();    nop(); break;
    case 0x5c: absolute_x();     nop(); break;
    case 0x5a: nop(); break;
    case 0x64: zero_page();      nop(); break;
    case 0x74: zero_page_x();    nop(); break;
    case 0x7c: absolute_x();     nop(); break;
    case 0x7a: nop(); break;
    case 0x80: immediate();      nop(); break;
    case 0x89: immediate();      nop(); break;
    case 0x82: immediate();      nop(); break;
    case 0xd4: zero_page_x();    nop(); break;
    case 0xdc: absolute_x();     nop(); break;
    case 0xc2: immediate();      nop(); break;
    case 0xda: nop(); break;
    case 0xf4: zero_page_x();    nop(); break;
    case 0xfc: absolute_x();     nop(); break;
    case 0xe2: immediate();      nop(); break;
    case 0xfa: nop(); break;
    default:
        return ERROR;
    }
    return SUCCESS;
}

}   // Namespace cpu.