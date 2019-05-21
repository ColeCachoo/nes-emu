// cpu.cpp : File contains class specific functions (like class constructor)
// and helper functions.
//
#include "cpu.h"

using namespace std;
using namespace std::placeholders;

CPU::CPU(RAM *ram) {
    program_counter	= 0x8000;
    stack_pointer	= 0xfd;
    accumulator		= 0x00;
    x_index			= 0x00;
    y_index			= 0x00;
    cpu_status		= 0x24;

    this->ram = ram;

    // TODO: This should get the starting address of the ROM.
    uint16_t start_pcl = ram->read(0xfffc);
    uint16_t start_pch = ram->read(0xfffd) << 8;
    program_counter = (start_pch | start_pcl);
    printf("Program Counter: %X\n", program_counter);
    program_counter = 0xc000;
    printf("Program Counter: %X\n", program_counter);

    // TODO: Maybe rename this or something.
    data_bus();

    // TODO: Get it to work without the full address on Windows.
    ofstream my_log("../resources/my_log.txt", ofstream::out | ofstream::trunc);
    // ofstream my_log("E:/Documents/programming/nes-emu/resources/my_log.txt", ofstream::out | ofstream::trunc);
    if (!my_log.is_open()) {
        puts("Failed to open my_log.txt");
    }

    // Run.
	current_op = 0;
    for (size_t i = 0; i < 8991; i++) {
        uint8_t opcode = fetch();
        current_op = opcode;
        println(my_log);
        // println();
        try {
            opcodes[opcode].op_func(opcodes[opcode].addr_mode);
        } catch(const std::bad_function_call &e) {
            break;
        }
    }

    my_log.close();
}

void 
CPU::stack_push(uint8_t val) {
    ram->write(val, STACK_BASE + stack_pointer);
    stack_pointer--;
}

uint8_t 
CPU::stack_pop() {
    stack_pointer++;
    return ram->read(STACK_BASE + stack_pointer);
}

void 
CPU::set_zero_if(uint8_t val) {
    if (val == 0) {
        cpu_status.set(Zero);
    } else {
        cpu_status.reset(Zero);
    }
}

void 
CPU::set_negative_if(uint8_t val) {
    if (val & 0x80) {
        cpu_status.set(Negative);
    } else {
        cpu_status.reset(Negative);
    }
}

uint8_t 
CPU::fetch() {
    uint8_t val = ram->read(program_counter);
    program_counter++;
    return val;
}

uint16_t 
CPU::fetch_with(Addressing addr_mode) {
    uint16_t addr = 0;
    switch (addr_mode) {
    case Addressing::Immediate :
        addr = immediate();
        break;
    case Addressing::ZeroPage :
        addr = zero_page();
        break;
    case Addressing::ZeroPageX :
        addr = zero_page_x();
        break;
    case Addressing::ZeroPageY :
        addr = zero_page_y();
        break;
    case Addressing::Relative :
        addr = relative();
        break;
    case Addressing::Absolute :
        addr = absolute();
        break;
    case Addressing::AbsoluteX :
        addr = absolute_x();
        break;
    case Addressing::AbsoluteY :
        addr = absolute_y();
        break;
    case Addressing::Indirect :
        addr = indirect();
        break;
    case Addressing::IndexedIndirect :
        addr = indexed_indirect();
        break;
    case Addressing::IndirectIndexed :
        addr = indirect_indexed();
        break;
    default :
        cerr << "Unexpected addressing mode in fetch_with(): " << int(addr_mode) << '\n';
    }

    return addr;
}

void
CPU::println() const {
    ios old_state(nullptr);
    old_state.copyfmt(cout);
    cout << setfill('0') << hex << uppercase <<
        setw(4) << program_counter - 1 << " " <<
        setw(2) << int(current_op) << " " <<
        "A:" << setw(2) << int(accumulator) << " " <<
        "X:" << setw(2) << int(x_index) << " " <<
        "Y:" << setw(2) << int(y_index) << " " <<
        "P:" << setw(2) << int(uint8_t(cpu_status.to_ulong())) << " " <<
        "SP:" << setw(2) << int(stack_pointer) << "\n";
    cout.copyfmt(old_state);
}

void
CPU::println(ofstream &my_log) const {
    my_log << setfill('0') << hex << uppercase <<
        setw(4) << program_counter - 1 << " " <<
        setw(2) << int(current_op) << " " <<
        "A:" << setw(2) << int(accumulator) << " " <<
        "X:" << setw(2) << int(x_index) << " " <<
        "Y:" << setw(2) << int(y_index) << " " <<
        "P:" << setw(2) << int(uint8_t(cpu_status.to_ulong())) << " " <<
        "SP:" << setw(2) << int(stack_pointer) << "\n";
}

void 
CPU::data_bus() {
    // LDA opcodes.
    opcodes.insert({ 0xa9, OpCode{ Addressing::Immediate,		2, 2, bind(&CPU::lda, this, _1) } });
    opcodes.insert({ 0xa5, OpCode{ Addressing::ZeroPage,		2, 3, bind(&CPU::lda, this, _1) } });
    opcodes.insert({ 0xb5, OpCode{ Addressing::ZeroPageX,		2, 4, bind(&CPU::lda, this, _1) } });
    opcodes.insert({ 0xad, OpCode{ Addressing::Absolute,		3, 4, bind(&CPU::lda, this, _1) } });
    opcodes.insert({ 0xbd, OpCode{ Addressing::AbsoluteX,		3, 4, bind(&CPU::lda, this, _1) } });
    opcodes.insert({ 0xb9, OpCode{ Addressing::AbsoluteY,		3, 4, bind(&CPU::lda, this, _1) } });
    opcodes.insert({ 0xa1, OpCode{ Addressing::IndexedIndirect,	2, 6, bind(&CPU::lda, this, _1) } });
    opcodes.insert({ 0xb1, OpCode{ Addressing::IndirectIndexed,	2, 5, bind(&CPU::lda, this, _1) } });
    // LDX opcodes.
    opcodes.insert({ 0xa2, OpCode{ Addressing::Immediate,		2, 2, bind(&CPU::ldx, this, _1) } });
    opcodes.insert({ 0xa6, OpCode{ Addressing::ZeroPage,		2, 3, bind(&CPU::ldx, this, _1) } });
    opcodes.insert({ 0xb6, OpCode{ Addressing::ZeroPageY,		2, 4, bind(&CPU::ldx, this, _1) } });
    opcodes.insert({ 0xae, OpCode{ Addressing::Absolute,		3, 4, bind(&CPU::ldx, this, _1) } });
    opcodes.insert({ 0xbe, OpCode{ Addressing::AbsoluteY,		3, 4, bind(&CPU::ldx, this, _1) } });
    // LDY opcodes.
    opcodes.insert({ 0xa0, OpCode{ Addressing::Immediate,		2, 2, bind(&CPU::ldy, this, _1) } });
    opcodes.insert({ 0xa4, OpCode{ Addressing::ZeroPage,		2, 3, bind(&CPU::ldy, this, _1) } });
    opcodes.insert({ 0xb4, OpCode{ Addressing::ZeroPageX,		2, 4, bind(&CPU::ldy, this, _1) } });
    opcodes.insert({ 0xac, OpCode{ Addressing::Absolute,		3, 4, bind(&CPU::ldy, this, _1) } });
    opcodes.insert({ 0xbc, OpCode{ Addressing::AbsoluteX,		3, 4, bind(&CPU::ldy, this, _1) } });
    // STA opcodes.
    opcodes.insert({ 0x85, OpCode{ Addressing::ZeroPage,		2, 3, bind(&CPU::sta, this, _1) } });
    opcodes.insert({ 0x95, OpCode{ Addressing::ZeroPageX,		2, 4, bind(&CPU::sta, this, _1) } });
    opcodes.insert({ 0x8d, OpCode{ Addressing::Absolute,		3, 4, bind(&CPU::sta, this, _1) } });
    opcodes.insert({ 0x9d, OpCode{ Addressing::AbsoluteX,		3, 5, bind(&CPU::sta, this, _1) } });
    opcodes.insert({ 0x99, OpCode{ Addressing::AbsoluteY,		3, 5, bind(&CPU::sta, this, _1) } });
    opcodes.insert({ 0x81, OpCode{ Addressing::IndexedIndirect, 2, 6, bind(&CPU::sta, this, _1) } });
    opcodes.insert({ 0x91, OpCode{ Addressing::IndirectIndexed, 2, 6, bind(&CPU::sta, this, _1) } });
    // STX opcodes.
    opcodes.insert({ 0x86, OpCode{ Addressing::ZeroPage,		2, 3, bind(&CPU::stx, this, _1) } });
    opcodes.insert({ 0x96, OpCode{ Addressing::ZeroPageY,		2, 4, bind(&CPU::stx, this, _1) } });
    opcodes.insert({ 0x8e, OpCode{ Addressing::Absolute,		3, 4, bind(&CPU::stx, this, _1) } });
    // STY opcodes.
    opcodes.insert({ 0x84, OpCode{ Addressing::ZeroPage,		2, 3, bind(&CPU::sty, this, _1) } });
    opcodes.insert({ 0x94, OpCode{ Addressing::ZeroPageX,		2, 4, bind(&CPU::sty, this, _1) } });
    opcodes.insert({ 0x8c, OpCode{ Addressing::Absolute,		3, 4, bind(&CPU::sty, this, _1) } });
    // TAX opcodes.
    opcodes.insert({ 0xaa, OpCode{ Addressing::Implicit,		1, 2, bind(&CPU::tax, this, _1) } });
    // TAY opcodes.
    opcodes.insert({ 0xa8, OpCode{ Addressing::Implicit,		1, 2, bind(&CPU::tay, this, _1) } });
    // TXA opcodes.
    opcodes.insert({ 0x8a, OpCode{ Addressing::Implicit,		1, 2, bind(&CPU::txa, this, _1) } });
    // TYA opcodes.
    opcodes.insert({ 0x98, OpCode{ Addressing::Implicit,		1, 2, bind(&CPU::tya, this, _1) } });
    // TSX opcodes.
    opcodes.insert({ 0xba, OpCode{ Addressing::Implicit,		1, 2, bind(&CPU::tsx, this, _1) } });
    // TXS opcodes.
    opcodes.insert({ 0x9a, OpCode{ Addressing::Implicit,		1, 2, bind(&CPU::txs, this, _1) } });
    // PHA opcodes.
    opcodes.insert({ 0x48, OpCode{ Addressing::Implicit,		1, 3, bind(&CPU::pha, this, _1) } });
    // PHP opcodes.
    opcodes.insert({ 0x08, OpCode{ Addressing::Implicit,		1, 3, bind(&CPU::php, this, _1) } });
    // PLA opcodes.
    opcodes.insert({ 0x68, OpCode{ Addressing::Implicit,		1, 4, bind(&CPU::pla, this, _1) } });
    // PLP opcodes.
    opcodes.insert({ 0x28, OpCode{ Addressing::Implicit,		1, 4, bind(&CPU::plp, this, _1) } });
    // AND opcodes.
    opcodes.insert({ 0x29, OpCode{ Addressing::Immediate,		2, 2, bind(&CPU::logical_and, this, _1) } });
    opcodes.insert({ 0x25, OpCode{ Addressing::ZeroPage,		2, 3, bind(&CPU::logical_and, this, _1) } });
    opcodes.insert({ 0x35, OpCode{ Addressing::ZeroPageX,		2, 4, bind(&CPU::logical_and, this, _1) } });
    opcodes.insert({ 0x2d, OpCode{ Addressing::Absolute,		3, 4, bind(&CPU::logical_and, this, _1) } });
    opcodes.insert({ 0x3d, OpCode{ Addressing::AbsoluteX,		3, 4, bind(&CPU::logical_and, this, _1) } });
    opcodes.insert({ 0x39, OpCode{ Addressing::AbsoluteY,		3, 4, bind(&CPU::logical_and, this, _1) } });
    opcodes.insert({ 0x21, OpCode{ Addressing::IndexedIndirect, 2, 6, bind(&CPU::logical_and, this, _1) } });
    opcodes.insert({ 0x31, OpCode{ Addressing::IndirectIndexed, 2, 5, bind(&CPU::logical_and, this, _1) } });
    // EOR opcodes.
    opcodes.insert({ 0x49, OpCode{ Addressing::Immediate,		2, 2, bind(&CPU::eor, this, _1) } });
    opcodes.insert({ 0x45, OpCode{ Addressing::ZeroPage,		2, 3, bind(&CPU::eor, this, _1) } });
    opcodes.insert({ 0x55, OpCode{ Addressing::ZeroPageX,		2, 4, bind(&CPU::eor, this, _1) } });
    opcodes.insert({ 0x4d, OpCode{ Addressing::Absolute,		3, 4, bind(&CPU::eor, this, _1) } });
    opcodes.insert({ 0x5d, OpCode{ Addressing::AbsoluteX,		3, 4, bind(&CPU::eor, this, _1) } });
    opcodes.insert({ 0x59, OpCode{ Addressing::AbsoluteY,		3, 4, bind(&CPU::eor, this, _1) } });
    opcodes.insert({ 0x41, OpCode{ Addressing::IndexedIndirect, 2, 6, bind(&CPU::eor, this, _1) } });
    opcodes.insert({ 0x51, OpCode{ Addressing::IndirectIndexed, 2, 5, bind(&CPU::eor, this, _1) } });
    // ORA opcodes.
    opcodes.insert({ 0x09, OpCode{ Addressing::Immediate,		2, 2, bind(&CPU::ora, this, _1) } });
    opcodes.insert({ 0x05, OpCode{ Addressing::ZeroPage,		2, 3, bind(&CPU::ora, this, _1) } });
    opcodes.insert({ 0x15, OpCode{ Addressing::ZeroPageX,		2, 4, bind(&CPU::ora, this, _1) } });
    opcodes.insert({ 0x0d, OpCode{ Addressing::Absolute,		3, 4, bind(&CPU::ora, this, _1) } });
    opcodes.insert({ 0x1d, OpCode{ Addressing::AbsoluteX,		3, 4, bind(&CPU::ora, this, _1) } });
    opcodes.insert({ 0x19, OpCode{ Addressing::AbsoluteY,		3, 4, bind(&CPU::ora, this, _1) } });
    opcodes.insert({ 0x01, OpCode{ Addressing::IndexedIndirect, 2, 6, bind(&CPU::ora, this, _1) } });
    opcodes.insert({ 0x11, OpCode{ Addressing::IndirectIndexed, 2, 5, bind(&CPU::ora, this, _1) } });
    // BIT opcodes.
    opcodes.insert({ 0x24, OpCode{ Addressing::ZeroPage,		2, 3, bind(&CPU::bit, this, _1) } });
    opcodes.insert({ 0x2c, OpCode{ Addressing::Absolute,		3, 4, bind(&CPU::bit, this, _1) } });
    // ADC opcodes.
    opcodes.insert({ 0x69, OpCode{ Addressing::Immediate,		2, 2, bind(&CPU::adc, this, _1) } });
    opcodes.insert({ 0x65, OpCode{ Addressing::ZeroPage,		2, 3, bind(&CPU::adc, this, _1) } });
    opcodes.insert({ 0x75, OpCode{ Addressing::ZeroPageX,		2, 4, bind(&CPU::adc, this, _1) } });
    opcodes.insert({ 0x6d, OpCode{ Addressing::Absolute,		3, 4, bind(&CPU::adc, this, _1) } });
    opcodes.insert({ 0x7d, OpCode{ Addressing::AbsoluteX,		3, 4, bind(&CPU::adc, this, _1) } });
    opcodes.insert({ 0x79, OpCode{ Addressing::AbsoluteY,		3, 4, bind(&CPU::adc, this, _1) } });
    opcodes.insert({ 0x61, OpCode{ Addressing::IndexedIndirect, 2, 6, bind(&CPU::adc, this, _1) } });
    opcodes.insert({ 0x71, OpCode{ Addressing::IndirectIndexed, 2, 5, bind(&CPU::adc, this, _1) } });
    // SBC opcodes.
    opcodes.insert({ 0xe9, OpCode{ Addressing::Immediate,		2, 2, bind(&CPU::sbc, this, _1) } });
    opcodes.insert({ 0xe5, OpCode{ Addressing::ZeroPage,		2, 3, bind(&CPU::sbc, this, _1) } });
    opcodes.insert({ 0xf5, OpCode{ Addressing::ZeroPageX,		2, 4, bind(&CPU::sbc, this, _1) } });
    opcodes.insert({ 0xed, OpCode{ Addressing::Absolute,		3, 4, bind(&CPU::sbc, this, _1) } });
    opcodes.insert({ 0xfd, OpCode{ Addressing::AbsoluteX,		3, 4, bind(&CPU::sbc, this, _1) } });
    opcodes.insert({ 0xf9, OpCode{ Addressing::AbsoluteY,		3, 4, bind(&CPU::sbc, this, _1) } });
    opcodes.insert({ 0xe1, OpCode{ Addressing::IndexedIndirect, 2, 6, bind(&CPU::sbc, this, _1) } });
    opcodes.insert({ 0xf1, OpCode{ Addressing::IndirectIndexed, 2, 5, bind(&CPU::sbc, this, _1) } });
    // CMP opcodes.
    opcodes.insert({ 0xc9, OpCode{ Addressing::Immediate,		2, 2, bind(&CPU::cmp, this, _1) } });
    opcodes.insert({ 0xc5, OpCode{ Addressing::ZeroPage,		2, 3, bind(&CPU::cmp, this, _1) } });
    opcodes.insert({ 0xd5, OpCode{ Addressing::ZeroPageX,		2, 4, bind(&CPU::cmp, this, _1) } });
    opcodes.insert({ 0xcd, OpCode{ Addressing::Absolute,		3, 4, bind(&CPU::cmp, this, _1) } });
    opcodes.insert({ 0xdd, OpCode{ Addressing::AbsoluteX,		3, 4, bind(&CPU::cmp, this, _1) } });
    opcodes.insert({ 0xd9, OpCode{ Addressing::AbsoluteY,		3, 4, bind(&CPU::cmp, this, _1) } });
    opcodes.insert({ 0xc1, OpCode{ Addressing::IndexedIndirect, 2, 6, bind(&CPU::cmp, this, _1) } });
    opcodes.insert({ 0xd1, OpCode{ Addressing::IndirectIndexed, 2, 5, bind(&CPU::cmp, this, _1) } });
    // CPX opcodes.
    opcodes.insert({ 0xe0, OpCode{ Addressing::Immediate,		2, 2, bind(&CPU::cpx, this, _1) } });
    opcodes.insert({ 0xe4, OpCode{ Addressing::ZeroPage,		2, 3, bind(&CPU::cpx, this, _1) } });
    opcodes.insert({ 0xec, OpCode{ Addressing::Absolute,		3, 4, bind(&CPU::cpx, this, _1) } });
    // CPY opcodes.
    opcodes.insert({ 0xc0, OpCode{ Addressing::Immediate,		2, 2, bind(&CPU::cpy, this, _1) } });
    opcodes.insert({ 0xc4, OpCode{ Addressing::ZeroPage,		2, 3, bind(&CPU::cpy, this, _1) } });
    opcodes.insert({ 0xcc, OpCode{ Addressing::Absolute,		3, 4, bind(&CPU::cpy, this, _1) } });
    // INC opcodes.
    opcodes.insert({ 0xe6, OpCode{ Addressing::ZeroPage,		2, 5, bind(&CPU::inc, this, _1) } });
    opcodes.insert({ 0xf6, OpCode{ Addressing::ZeroPageX,		2, 6, bind(&CPU::inc, this, _1) } });
    opcodes.insert({ 0xee, OpCode{ Addressing::Absolute,		3, 6, bind(&CPU::inc, this, _1) } });
    opcodes.insert({ 0xfe, OpCode{ Addressing::AbsoluteX,		3, 7, bind(&CPU::inc, this, _1) } });
    // INX opcodes.
    opcodes.insert({ 0xe8, OpCode{ Addressing::Implicit,		1, 2, bind(&CPU::inx, this, _1) } });
    // INY opcodes.
    opcodes.insert({ 0xc8, OpCode{ Addressing::Implicit,		1, 2, bind(&CPU::iny, this, _1) } });
    // DEC opcodes.
    opcodes.insert({ 0xc6, OpCode{ Addressing::ZeroPage,		2, 5, bind(&CPU::dec, this, _1) } });
    opcodes.insert({ 0xd6, OpCode{ Addressing::ZeroPageX,		2, 6, bind(&CPU::dec, this, _1) } });
    opcodes.insert({ 0xce, OpCode{ Addressing::Absolute,		3, 6, bind(&CPU::dec, this, _1) } });
    opcodes.insert({ 0xde, OpCode{ Addressing::AbsoluteX,		3, 7, bind(&CPU::dec, this, _1) } });
    // DEX opcodes.
    opcodes.insert({ 0xca, OpCode{ Addressing::Implicit,		1, 2, bind(&CPU::dex, this, _1) } });
    // DEY opcodes.
    opcodes.insert({ 0x88, OpCode{ Addressing::Implicit,		1, 2, bind(&CPU::dey, this, _1) } });
    // ASL opcodes.
    opcodes.insert({ 0x0a, OpCode{ Addressing::Accumulator,		1, 2, bind(&CPU::asl, this, _1) } });
    opcodes.insert({ 0x06, OpCode{ Addressing::ZeroPage,		2, 5, bind(&CPU::asl, this, _1) } });
    opcodes.insert({ 0x16, OpCode{ Addressing::ZeroPageX,		2, 6, bind(&CPU::asl, this, _1) } });
    opcodes.insert({ 0x0e, OpCode{ Addressing::Absolute,		3, 6, bind(&CPU::asl, this, _1) } });
    opcodes.insert({ 0x1e, OpCode{ Addressing::AbsoluteX,		3, 7, bind(&CPU::asl, this, _1) } });
    // LSR opcodes.
    opcodes.insert({ 0x4a, OpCode{ Addressing::Accumulator,		1, 2, bind(&CPU::lsr, this, _1) } });
    opcodes.insert({ 0x46, OpCode{ Addressing::ZeroPage,		2, 5, bind(&CPU::lsr, this, _1) } });
    opcodes.insert({ 0x56, OpCode{ Addressing::ZeroPageX,		2, 6, bind(&CPU::lsr, this, _1) } });
    opcodes.insert({ 0x4e, OpCode{ Addressing::Absolute,		3, 6, bind(&CPU::lsr, this, _1) } });
    opcodes.insert({ 0x5e, OpCode{ Addressing::AbsoluteX,		3, 7, bind(&CPU::lsr, this, _1) } });
    // ROL opcodes.
    opcodes.insert({ 0x2a, OpCode{ Addressing::Accumulator,		1, 2, bind(&CPU::rol, this, _1) } });
    opcodes.insert({ 0x26, OpCode{ Addressing::ZeroPage,		2, 5, bind(&CPU::rol, this, _1) } });
    opcodes.insert({ 0x36, OpCode{ Addressing::ZeroPageX,		2, 6, bind(&CPU::rol, this, _1) } });
    opcodes.insert({ 0x2e, OpCode{ Addressing::Absolute,		3, 6, bind(&CPU::rol, this, _1) } });
    opcodes.insert({ 0x3e, OpCode{ Addressing::AbsoluteX,		3, 7, bind(&CPU::rol, this, _1) } });
    // ROR opcodes.
    opcodes.insert({ 0x6a, OpCode{ Addressing::Accumulator,		1, 2, bind(&CPU::ror, this, _1) } });
    opcodes.insert({ 0x66, OpCode{ Addressing::ZeroPage,		2, 5, bind(&CPU::ror, this, _1) } });
    opcodes.insert({ 0x76, OpCode{ Addressing::ZeroPageX,		2, 6, bind(&CPU::ror, this, _1) } });
    opcodes.insert({ 0x6e, OpCode{ Addressing::Absolute,		3, 6, bind(&CPU::ror, this, _1) } });
    opcodes.insert({ 0x7e, OpCode{ Addressing::AbsoluteX,		3, 7, bind(&CPU::ror, this, _1) } });
    // JMP opcodes.
    opcodes.insert({ 0x4c, OpCode{ Addressing::Absolute,		3, 3, bind(&CPU::jmp, this, _1) } });
    opcodes.insert({ 0x6c, OpCode{ Addressing::Indirect,		3, 5, bind(&CPU::jmp, this, _1) } });
    // JSR opcodes.
    opcodes.insert({ 0x20, OpCode{ Addressing::Absolute,		3, 6, bind(&CPU::jsr, this, _1) } });
    // RTS opcodes.
    opcodes.insert({ 0x60, OpCode{ Addressing::Implicit,		1, 6, bind(&CPU::rts, this, _1) } });
    // BCC opcodes.
    opcodes.insert({ 0x90, OpCode{ Addressing::Relative,		2, 2, bind(&CPU::bcc, this, _1) } });
    // BCS opcodes.
    opcodes.insert({ 0xb0, OpCode{ Addressing::Relative,		2, 2, bind(&CPU::bcs, this, _1) } });
    // BEQ opcodes.
    opcodes.insert({ 0xf0, OpCode{ Addressing::Relative,		2, 2, bind(&CPU::beq, this, _1) } });
    // BMI opcodes.
    opcodes.insert({ 0x30, OpCode{ Addressing::Relative,		2, 2, bind(&CPU::bmi, this, _1) } });
    // BNE opcodes.
    opcodes.insert({ 0xd0, OpCode{ Addressing::Relative,		2, 2, bind(&CPU::bne, this, _1) } });
    // BPL opcodes.
    opcodes.insert({ 0x10, OpCode{ Addressing::Relative,		2, 2, bind(&CPU::bpl, this, _1) } });
    // BVC opcodes.
    opcodes.insert({ 0x50, OpCode{ Addressing::Relative,		2, 2, bind(&CPU::bvc, this, _1) } });
    // BVS opcodes.
    opcodes.insert({ 0x70, OpCode{ Addressing::Relative,		2, 2, bind(&CPU::bvs, this, _1) } });
    // CLC opcodes.
    opcodes.insert({ 0x18, OpCode{ Addressing::Implicit,		1, 2, bind(&CPU::clc, this, _1) } });
    // CLD opcodes.
    opcodes.insert({ 0xd8, OpCode{ Addressing::Implicit,		1, 2, bind(&CPU::cld, this, _1) } });
    // CLI opcodes.
    opcodes.insert({ 0x58, OpCode{ Addressing::Implicit,		1, 2, bind(&CPU::cli, this, _1) } });
    // CLV opcodes.
    opcodes.insert({ 0xb8, OpCode{ Addressing::Implicit,		1, 2, bind(&CPU::clv, this, _1) } });
    // SEC opcodes.
    opcodes.insert({ 0x38, OpCode{ Addressing::Implicit,		1, 2, bind(&CPU::sec, this, _1) } });
    // SED opcodes.
    opcodes.insert({ 0xf8, OpCode{ Addressing::Implicit,		1, 2, bind(&CPU::sed, this, _1) } });
    // SEI opcodes.
    opcodes.insert({ 0x78, OpCode{ Addressing::Implicit,		1, 2, bind(&CPU::sei, this, _1) } });
    // BRK opcodes.
    opcodes.insert({ 0x00, OpCode{ Addressing::Implicit,		1, 7, bind(&CPU::brk, this, _1) } });
    // NOP opcodes.
    opcodes.insert({ 0xea, OpCode{ Addressing::Implicit,		1, 2, bind(&CPU::nop, this, _1) } });
    // RTI opcodes.
    opcodes.insert({ 0x40, OpCode{ Addressing::Implicit,		1, 6, bind(&CPU::rti, this, _1) } });
    // TODO: For these I need to find out cycle times. Right now I'm just
    // copying them from an opcode above that has the same addressing mode.
    // Additional NOP opcodes.
    opcodes.insert({ 0x04, OpCode{ Addressing::ZeroPage,		2, 3, bind(&CPU::nop, this, _1) } });
    opcodes.insert({ 0x0c, OpCode{ Addressing::Absolute,		3, 4, bind(&CPU::nop, this, _1) } });
    opcodes.insert({ 0x14, OpCode{ Addressing::ZeroPageX,		2, 4, bind(&CPU::nop, this, _1) } });
    opcodes.insert({ 0x1c, OpCode{ Addressing::AbsoluteX,		3, 4, bind(&CPU::nop, this, _1) } });
    opcodes.insert({ 0x1a, OpCode{ Addressing::Implicit,		1, 2, bind(&CPU::nop, this, _1) } });
    opcodes.insert({ 0x34, OpCode{ Addressing::ZeroPageX,		2, 4, bind(&CPU::nop, this, _1) } });
    opcodes.insert({ 0x3c, OpCode{ Addressing::AbsoluteX,		3, 4, bind(&CPU::nop, this, _1) } });
    opcodes.insert({ 0x3a, OpCode{ Addressing::Implicit,		1, 2, bind(&CPU::nop, this, _1) } });
    opcodes.insert({ 0x44, OpCode{ Addressing::ZeroPage,		2, 3, bind(&CPU::nop, this, _1) } });
    opcodes.insert({ 0x54, OpCode{ Addressing::ZeroPageX,		2, 4, bind(&CPU::nop, this, _1) } });
    opcodes.insert({ 0x5c, OpCode{ Addressing::AbsoluteX,		3, 4, bind(&CPU::nop, this, _1) } });
    opcodes.insert({ 0x5a, OpCode{ Addressing::Implicit,		1, 2, bind(&CPU::nop, this, _1) } });
    opcodes.insert({ 0x64, OpCode{ Addressing::ZeroPage,		2, 3, bind(&CPU::nop, this, _1) } });
    opcodes.insert({ 0x74, OpCode{ Addressing::ZeroPageX,		2, 4, bind(&CPU::nop, this, _1) } });
    opcodes.insert({ 0x7c, OpCode{ Addressing::AbsoluteX,		3, 4, bind(&CPU::nop, this, _1) } });
    opcodes.insert({ 0x7a, OpCode{ Addressing::Implicit,		1, 2, bind(&CPU::nop, this, _1) } });
    opcodes.insert({ 0x80, OpCode{ Addressing::Immediate,		2, 2, bind(&CPU::nop, this, _1) } });
    opcodes.insert({ 0x89, OpCode{ Addressing::Immediate,		2, 2, bind(&CPU::nop, this, _1) } });
    opcodes.insert({ 0x82, OpCode{ Addressing::Immediate,		2, 2, bind(&CPU::nop, this, _1) } });
    opcodes.insert({ 0xd4, OpCode{ Addressing::ZeroPageX,		2, 4, bind(&CPU::nop, this, _1) } });
    opcodes.insert({ 0xdc, OpCode{ Addressing::AbsoluteX,		3, 4, bind(&CPU::nop, this, _1) } });
    opcodes.insert({ 0xc2, OpCode{ Addressing::Immediate,		2, 2, bind(&CPU::nop, this, _1) } });
    opcodes.insert({ 0xda, OpCode{ Addressing::Implicit,		1, 2, bind(&CPU::nop, this, _1) } });
    opcodes.insert({ 0xf4, OpCode{ Addressing::ZeroPageX,		2, 4, bind(&CPU::nop, this, _1) } });
    opcodes.insert({ 0xfc, OpCode{ Addressing::AbsoluteX,		3, 4, bind(&CPU::nop, this, _1) } });
    opcodes.insert({ 0xe2, OpCode{ Addressing::Immediate,		2, 2, bind(&CPU::nop, this, _1) } });
    opcodes.insert({ 0xfa, OpCode{ Addressing::Implicit,		1, 2, bind(&CPU::nop, this, _1) } });
}

