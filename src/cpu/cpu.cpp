// cpu.cpp : File contains class specific functions (like class constructor)
// and helper functions.
//
#include "cpu.h"

using namespace std;
using namespace std::placeholders;

CPU::CPU(RAM *ram) {
	program_counter	= 0x8000;
	stack_pointer	= 0x00;
	accumulator		= 0x00;
	x_index			= 0x00;
	y_index			= 0x00;
	cpu_status		= 0x34;

	this->ram = ram;

	// TODO: Phase out.
	memory = vector<uint8_t>(64 * 1024);

	data_bus();
	lda(Addressing::ZeroPageX);

	program_counter = 0x1234;
	printf("%x%x\n", get_pch(), get_pcl());
	jsr(0x5678);
	printf("%x\n", program_counter);
	rts();
	printf("%x\n", program_counter);
	printf("%x\n", 0x80 + 0x0f);

	// Tests ADC.
	puts("Testing adc...");
	program_counter = 0x8000;
	cpu_status.reset();
	//cpu_status.set(0);
	accumulator = -66;
	ram->write(-65, 0x8000);
	adc(Addressing::Immediate);
	cout << int(int8_t(accumulator)) << endl;
	cout << cpu_status << endl;
	puts("Done.");
}

void 
CPU::stack_push(uint8_t val) {
	ram->write(val, STACK_BASE - stack_pointer);
	stack_pointer++;
}

uint8_t 
CPU::stack_pop() {
	stack_pointer--;
	return ram->read(STACK_BASE - stack_pointer);
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
	bitset<8> v = val;
	if (v[7] == 1) {
		cpu_status.set(Negative);
	} else {
		cpu_status.reset(Negative);
	}
}

// TODO: See JMP in the reference. The 6502 doesn't correctly fetch
// address if it falls on a page boundry?
uint8_t 
CPU::fetch() {
	uint8_t val = ram->read(program_counter);
	program_counter++;
	return val;
}

void 
CPU::data_bus() {
	unordered_map<uint8_t, OpCode> opcodes;
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
	opcodes.insert({ 0x96, OpCode{ Addressing::ZeroPageX,		2, 4, bind(&CPU::stx, this, _1) } });
	opcodes.insert({ 0x8e, OpCode{ Addressing::Absolute,		3, 4, bind(&CPU::stx, this, _1) } });
	// STY opcodes.
	opcodes.insert({ 0x84, OpCode{ Addressing::ZeroPage,		2, 3, bind(&CPU::sta, this, _1) } });
	opcodes.insert({ 0x94, OpCode{ Addressing::ZeroPageX,		2, 4, bind(&CPU::sta, this, _1) } });
	opcodes.insert({ 0x8c, OpCode{ Addressing::Absolute,		3, 4, bind(&CPU::sta, this, _1) } });
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
	opcodes.insert({ 0x28, OpCode{ Addressing::Implicit,		1, 4, bind(&CPU::tax, this, _1) } });
	// AND opcodes.
	opcodes.insert({ 0x29, OpCode{ Addressing::Immediate,		2, 2, bind(&CPU::logical_and, this, _1) } });
	opcodes.insert({ 0x25, OpCode{ Addressing::ZeroPage,		2, 3, bind(&CPU::logical_and, this, _1) } });
	opcodes.insert({ 0x35, OpCode{ Addressing::ZeroPageX,		2, 4, bind(&CPU::logical_and, this, _1) } });
	opcodes.insert({ 0x2d, OpCode{ Addressing::Absolute,		3, 4, bind(&CPU::logical_and, this, _1) } });
	opcodes.insert({ 0x3d, OpCode{ Addressing::AbsoluteX,		3, 4, bind(&CPU::logical_and, this, _1) } });
	opcodes.insert({ 0x39, OpCode{ Addressing::AbsoluteY,		3, 4, bind(&CPU::logical_and, this, _1) } });
	opcodes.insert({ 0x21, OpCode{ Addressing::IndexedIndirect, 2, 6, bind(&CPU::logical_and, this, _1) } });
	opcodes.insert({ 0x11, OpCode{ Addressing::IndirectIndexed, 2, 5, bind(&CPU::logical_and, this, _1) } });
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

	//opcodes.at(0xa9).op_func(opcodes.at(0xa9).addr_mode);
	//opcodes.at(0xa0).op_func(Addressing::Indirect);
}

