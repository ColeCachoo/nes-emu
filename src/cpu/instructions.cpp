// instructions.cpp : File contains only the cpu instructions.
//
#include "cpu.h"

#include <iostream>

using namespace std;

/*************************
 * Load/Store Operations *
 *************************/
/// LDA - Load Accumulator
void 
CPU::lda(Addressing addr_mode) {
	uint8_t val = 0;
	switch (addr_mode) {
	case Addressing::Immediate :
		val = immediate();
		break;
	case Addressing::ZeroPage :
		val = zero_page();
		break;
	case Addressing::ZeroPageX :
		val = zero_page_x();
		break;
	case Addressing::Absolute :
		val = absolute();
		break;
	case Addressing::AbsoluteX :
		val = absolute_x();
		break;
	case Addressing::AbsoluteY :
		val = absolute_y();
		break;
	case Addressing::IndexedIndirect :
		val = indexed_indirect();
		break;
	case Addressing::IndirectIndexed :
		val = indirect_indexed();
		break;
	default :
		cerr << "Unexpected addressing mode in LDA: " << int(addr_mode) << endl;
	}

	accumulator = val;
	set_zero_if(accumulator);
	set_negative_if(accumulator);
}

/// LDX - Load X Register
void
CPU::ldx(Addressing addr_mode) {
	uint8_t val = 0;
	switch (addr_mode) {
	case Addressing::Immediate :
		val = immediate();
		break;
	case Addressing::ZeroPage :
		val = zero_page();
		break;
	case Addressing::ZeroPageY :
		val = zero_page_y();
		break;
	case Addressing::Absolute :
		val = absolute();
		break;
	case Addressing::AbsoluteY :
		val = absolute_y();
		break;
	default :
		cerr << "Unexpected addressing mode in LDX: " << int(addr_mode) << endl;
	}

	x_index = val;
	set_zero_if(x_index);
	set_negative_if(x_index);
}

/// LDY - Load Y Register
void
CPU::ldy(Addressing addr_mode) {
	uint8_t val = 0;
	switch (addr_mode) {
	case Addressing::Immediate :
		val = immediate();
		break;
	case Addressing::ZeroPage :
		val = zero_page();
		break;
	case Addressing::ZeroPageX :
		val = zero_page_x();
		break;
	case Addressing::Absolute :
		val = absolute();
		break;
	case Addressing::AbsoluteX :
		val = absolute_x();
		break;
	default :
		cerr << "Unexpected addressing mode in LDY: " << int(addr_mode) << endl;
	}

	y_index = val;
	set_zero_if(y_index);
	set_negative_if(y_index);
}

/// STA - Store Accumulator
void 
CPU::sta(Addressing addr_mode) {
	uint8_t val = 0;
	switch (addr_mode) {
	case Addressing::ZeroPage :
		val = zero_page();
		break;
	case Addressing::ZeroPageX :
		val = zero_page_x();
		break;
	case Addressing::Absolute :
		val = absolute();
		break;
	case Addressing::AbsoluteX :
		val = absolute_x();
		break;
	case Addressing::AbsoluteY :
		val = absolute_y();
		break;
	case Addressing::IndexedIndirect :
		val = indexed_indirect();
		break;
	case Addressing::IndirectIndexed :
		val = indirect_indexed();
		break;
	default :
		cerr << "Unexpected addressing mode in STA: " << int(addr_mode) << endl;
	}

	ram->write(accumulator, val);
}

/// STX - Store X Register
void 
CPU::stx(Addressing addr_mode) {
	uint8_t val = 0;
	switch (addr_mode) {
	case Addressing::ZeroPage :
		val = zero_page();
		break;
	case Addressing::ZeroPageY :
		val = zero_page_y();
		break;
	case Addressing::Absolute :
		val = absolute();
		break;
	default :
		cerr << "Unexpected addressing mode in STX: " << int(addr_mode) << endl;
	}

	ram->write(x_index, val);
}

/// STY - Store Y Register
void 
CPU::sty(Addressing addr_mode) {
	uint8_t val = 0;
	switch (addr_mode) {
	case Addressing::ZeroPage :
		val = zero_page();
		break;
	case Addressing::ZeroPageX :
		val = zero_page_x();
		break;
	case Addressing::Absolute :
		val = absolute();
		break;
	default :
		cerr << "Unexpected addressing mode in STY: " << int(addr_mode) << endl;
	}

	ram->write(y_index, val);
}

/**********************
 * Register Transfers *
 **********************/
/// TAX - Transfer Accumulator to X
void 
CPU::tax(Addressing addr_mode) {
	switch (addr_mode) {
	case Addressing::Implicit :
		x_index = accumulator;
		break;
	default :
		cerr << "Unexpected addressing mode in TAX: " << int(addr_mode) << endl;
	}

	set_zero_if(x_index);
	set_negative_if(x_index);
}

/// TAY - Transfer Accumulator to Y
void 
CPU::tay(Addressing addr_mode) {
	switch (addr_mode) {
	case Addressing::Implicit :
		y_index = accumulator;
		break;
	default :
		cerr << "Unexpected addressing mode in TAY: " << int(addr_mode) << endl;
	}

	set_zero_if(y_index);
	set_negative_if(y_index);
}

/// TXA - Transfer Accumulator to X
void 
CPU::txa(Addressing addr_mode) {
	switch (addr_mode) {
	case Addressing::Implicit :
		accumulator = x_index;
		break;
	default :
		cerr << "Unexpected addressing mode in TXA: " << int(addr_mode) << endl;
	}

	set_zero_if(accumulator);
	set_negative_if(accumulator);
}

/// TYA - Transfer Accumulator to X
void 
CPU::tya(Addressing addr_mode) {
	switch (addr_mode) {
	case Addressing::Implicit :
		accumulator = y_index;
		break;
	default :
		cerr << "Unexpected addressing mode in TYA: " << int(addr_mode) << endl;
	}

	set_zero_if(accumulator);
	set_negative_if(accumulator);
}

/********************
 * Stack Operations *
 ********************/
/// TSX - Transfer stack pointer to X	
void 
CPU::tsx(Addressing addr_mode) {
	switch (addr_mode) {
	case Addressing::Implicit :
		x_index = stack_pop();
		break;
	default :
		cerr << "Unexpected addressing mode in TSX: " << int(addr_mode) << endl;
	}

	set_zero_if(x_index);
	set_negative_if(x_index);
}

/// TXS - Transfer X to stack pointer	
void 
CPU::txs(Addressing addr_mode) {
	switch (addr_mode) {
	case Addressing::Implicit :
		stack_push(x_index);
		break;
	default :
		cerr << "Unexpected addressing mode in TXS: " << int(addr_mode) << endl;
	}
}

/// PHA - Push accumulator on stack	
void 
CPU::pha(Addressing addr_mode) {
	switch (addr_mode) {
	case Addressing::Implicit :
		stack_push(accumulator);
		break;
	default :
		cerr << "Unexpected addressing mode in PHA: " << int(addr_mode) << endl;
	}
}

/// PHP - Push processor status on stack	
void 
CPU::php(Addressing addr_mode) {
	switch (addr_mode) {
	case Addressing::Implicit :
		stack_push(uint8_t(cpu_status.to_ulong()));
		break;
	default :
		cerr << "Unexpected addressing mode in PHP: " << int(addr_mode) << endl;
	}
}

/// PLA - Pull accumulator from stack	
void 
CPU::pla(Addressing addr_mode) {
	switch (addr_mode) {
	case Addressing::Implicit :
		accumulator = stack_pop();
		break;
	default :
		cerr << "Unexpected addressing mode in PLA: " << int(addr_mode) << endl;
	}

	set_zero_if(accumulator);
	set_negative_if(accumulator);
}

/// PLP - Pull processor status from stack	
void 
CPU::plp(Addressing addr_mode) {
	switch (addr_mode) {
	case Addressing::Implicit :
		cpu_status = stack_pop();
		break;
	default :
		cerr << "Unexpected addressing mode in PLP: " << int(addr_mode) << endl;
	}
}

/***********
 * Logical *
 ***********/
/// AND - Logical AND
void 
CPU::logical_and(Addressing addr_mode) {
	uint8_t val = 0;
	switch (addr_mode) {
	case Addressing::Immediate :
		val = immediate();
		break;
	case Addressing::ZeroPage :
		val = zero_page();
		break;
	case Addressing::ZeroPageX :
		val = zero_page_x();
		break;
	case Addressing::Absolute :
		val = absolute();
		break;
	case Addressing::AbsoluteX :
		val = absolute_x();
		break;
	case Addressing::AbsoluteY :
		val = absolute_y();
		break;
	case Addressing::IndexedIndirect :
		val = indexed_indirect();
		break;
	case Addressing::IndirectIndexed :
		val = indirect_indexed();
		break;
	default :
		cerr << "Unexpected addressing mode in AND: " << int(addr_mode) << endl;
	}

	accumulator &= val;
	set_zero_if(accumulator);
	set_negative_if(accumulator);
}

/// EOR - Exclusive OR
void 
CPU::eor(Addressing addr_mode) {
	uint8_t val = 0;
	switch (addr_mode) {
	case Addressing::Immediate :
		val ^= immediate();
		break;
	case Addressing::ZeroPage :
		val ^= zero_page();
		break;
	case Addressing::ZeroPageX :
		val ^= zero_page_x();
		break;
	case Addressing::Absolute :
		val ^= absolute();
		break;
	case Addressing::AbsoluteX :
		val ^= absolute_x();
		break;
	case Addressing::AbsoluteY :
		val ^= absolute_y();
		break;
	case Addressing::IndexedIndirect :
		val ^= indexed_indirect();
		break;
	case Addressing::IndirectIndexed :
		val ^= indirect_indexed();
		break;
	default :
		cerr << "Unexpected addressing mode in EOR: " << int(addr_mode) << endl;
	}

	accumulator ^= val;
	set_zero_if(accumulator);
	set_negative_if(accumulator);
}

/// ORA - Logical Inclusive OR
void
CPU::ora(Addressing addr_mode) {
	uint8_t val = 0;
	switch (addr_mode) {
	case Addressing::Immediate :
		val = immediate();
		break;
	case Addressing::ZeroPage :
		val = zero_page();
		break;
	case Addressing::ZeroPageX :
		val = zero_page_x();
		break;
	case Addressing::Absolute :
		val = absolute();
		break;
	case Addressing::AbsoluteX :
		val = absolute_x();
		break;
	case Addressing::AbsoluteY :
		val = absolute_y();
		break;
	case Addressing::IndexedIndirect :
		val = indexed_indirect();
		break;
	case Addressing::IndirectIndexed :
		val = indirect_indexed();
		break;
	default :
		cerr << "Unexpected addressing mode in ORA: " << int(addr_mode) << endl;
	}

	accumulator |= val;
	set_zero_if(accumulator);
	set_negative_if(accumulator);
}

/// BIT - Bit Test
void 
CPU::bit(Addressing addr_mode) {
	uint8_t val = 0;
	bitset<8> v  = 0;
	switch (addr_mode) {
	case Addressing::ZeroPage :
		val = zero_page();
		v = zero_page();
		break;
	case Addressing::Absolute :
		val = absolute();
		v = absolute();
		break;
	default :
		cerr << "Unexpected addressing mode in BIT: " << int(addr_mode) << endl;
	}

	set_zero_if(accumulator & val);
	cpu_status.set(Overflow, v[6]);
	cpu_status.set(Negative, v[7]);
}

/**************
 * Arithmetic *
 **************/
/// ADC - Add with Carry
void 
CPU::adc(Addressing addr_mode) {
	uint8_t val = 0;
	switch (addr_mode) {
	case Addressing::Immediate :
		val = immediate();
		break;
	case Addressing::ZeroPage :
		val = zero_page();
		break;
	case Addressing::ZeroPageX :
		val = zero_page_x();
		break;
	case Addressing::Absolute :
		val = absolute();
		break;
	case Addressing::AbsoluteX :
		val = absolute_x();
		break;
	case Addressing::AbsoluteY :
		val = absolute_y();
		break;
	case Addressing::IndexedIndirect :
		val = indexed_indirect();
		break;
	case Addressing::IndirectIndexed :
		val = indirect_indexed();
		break;
	default :
		cerr << "Unexpected addressing mode in ADC: " << int(addr_mode) << endl;
	}

	// Used later to check for overflow. Needs accumulator before it gets changed.
	uint8_t old_a = accumulator;

	uint16_t result = accumulator;
	uint8_t carry_bit;
	if (cpu_status[Carry] == 1) {
		carry_bit = 0x01;
	} else {
		carry_bit = 0x00;
	}
	result = result + val + carry_bit;

	accumulator = uint8_t(result);
	set_zero_if(accumulator);
	set_negative_if(accumulator);
	// Set carry if ninth position bit is set.
	if ((result & 0x0100) != 0) {
		cpu_status.set(Carry);
	} else {
		cpu_status.reset(Carry);
	}

	// Check for overflow and set cpu_status if needed.
	if (((old_a ^ val) & 0x80) != 0) {
		cpu_status.reset(Overflow);
	} else if ((old_a & val & 0x80) == (result & 0x80)) {
		cpu_status.reset(Overflow);
	} else {
		cpu_status.set(Overflow);
	}
}

/// SBC - Subtract with Carry
void 
CPU::sbc(Addressing addr_mode) {
	uint8_t val = 0;
	switch (addr_mode) {
	case Addressing::Immediate :
		val = immediate();
		break;
	case Addressing::ZeroPage :
		val = zero_page();
		break;
	case Addressing::ZeroPageX :
		val = zero_page_x();
		break;
	case Addressing::Absolute :
		val = absolute();
		break;
	case Addressing::AbsoluteX :
		val = absolute_x();
		break;
	case Addressing::AbsoluteY :
		val = absolute_y();
		break;
	case Addressing::IndexedIndirect :
		val = indexed_indirect();
		break;
	case Addressing::IndirectIndexed :
		val = indirect_indexed();
		break;
	default :
		cerr << "Unexpected addressing mode in SBC: " << int(addr_mode) << endl;
	}

	uint16_t result = accumulator;
	uint16_t status = cpu_status.to_ulong();
	// Following operations copy the carry flag from cpu status to result.
	status <<= 8;
	status |= 0x00ff;
	result |= 0x0100;
	result &= status;
	result = result - val - (1 - 0x0100);

	//accumulator = uint8_t(a.to_ulong());
	set_zero_if(accumulator);
	set_negative_if(accumulator);
	//if (a[8] == 0) {		// Overflow in bit 7.
	if (accumulator == 0) {		// Overflow in bit 7.
		cpu_status.reset(Carry);
		cpu_status.set(Overflow);
	} else {
		cpu_status.set(Carry);
		cpu_status.reset(Overflow);
	}
}

/// CMP - Compare
void 
CPU::cmp(Addressing addr_mode) {
	uint16_t addr = 0;
	if (accumulator >= memory.at(addr)) {
		cpu_status.set(Carry);
	} else {
		cpu_status.reset(Carry);
	}

	if (accumulator == memory.at(addr)) {
		cpu_status.set(Zero);
	} else {
		cpu_status.reset(Zero);
	}

	if (((accumulator - memory.at(addr)) & 0x80) != 0) {
		cpu_status.set(Negative);
	} else {
		cpu_status.reset(Negative);
	}
}

/// CPX - Compare X Register
void 
CPU::cpx(Addressing addr_mode) {
	uint16_t addr = 0;
	if (x_index >= memory.at(addr)) {
		cpu_status.set(Carry);
	} else {
		cpu_status.reset(Carry);
	}

	if (x_index == memory.at(addr)) {
		cpu_status.set(Zero);
	} else {
		cpu_status.reset(Zero);
	}

	if (((x_index - memory.at(addr)) & 0x80) != 0) {
		cpu_status.set(Negative);
	} else {
		cpu_status.reset(Negative);
	}
}

/// CPY - Compare Y Register
void 
CPU::cpy(Addressing addr_mode) {
	uint16_t addr = 0;
	if (y_index >= memory.at(addr)) {
		cpu_status.set(Carry);
	} else {
		cpu_status.reset(Carry);
	}

	if (y_index == memory.at(addr)) {
		cpu_status.set(Zero);
	} else {
		cpu_status.reset(Zero);
	}

	if (((y_index - memory.at(addr)) & 0x80) != 0) {
		cpu_status.set(Negative);
	} else {
		cpu_status.reset(Negative);
	}
}

/***************************
 * Increments & Decrements *
 ***************************/
/// INC - Increment a memory location
void 
CPU::inc(const uint16_t &addr) {
	memory.at(addr)++;
	set_zero_if(memory.at(addr));
	set_negative_if(memory.at(addr));
}

/// INX - Increment the X register
void 
CPU::inx() {
	x_index++;
	set_zero_if(x_index);
	set_negative_if(x_index);
}

/// INY - Increment the Y register
void 
CPU::iny() {
	y_index++;
	set_zero_if(y_index);
	set_negative_if(y_index);
}

/// DEC - Decrement a memory location
void 
CPU::dec(const uint16_t &addr) {
	memory.at(addr)--;
	set_zero_if(memory.at(addr));
	set_negative_if(memory.at(addr));
}

/// DEX - Decrement the X register
void 
CPU::dex() {
	x_index--;
	set_zero_if(x_index);
	set_negative_if(x_index);
}

/// DEY - Decrement the Y register
void 
CPU::dey() {
	y_index--;
	set_zero_if(y_index);
	set_negative_if(y_index);
}

/**********
 * Shifts *
 **********/
/// ASL - 	Arithmetic Shift Left
void 
CPU::asl(uint8_t &val) {
	cpu_status.set(Carry, (val & 0x80) != 0);
	val = val << 1;
	set_zero_if(val);
	set_negative_if(val);
}

/// LSR - Logical Shift Right
void 
CPU::lsr(uint8_t &val) {
	cpu_status.set(Carry, (val & 0x01) != 0);
	val = val >> 1;
	set_zero_if(val);
	set_negative_if(val);
}

/// ROL - Rotate Left
void 
CPU::rol(uint8_t &val) {
	bitset<9> v = val;
	v = v << 1;
	v[0] = cpu_status[Carry];
	cpu_status.set(Carry, v[8]);

	val = uint8_t(v.to_ulong());
	set_zero_if(val);
	set_negative_if(val);
}

/// ROR - Rotate Right
void 
CPU::ror(uint8_t &val) {
	bitset<9> v = val;
	v[8] = cpu_status[Carry];
	cpu_status.set(Carry, v[0]);
	v = v >> 1;

	val = uint8_t(v.to_ulong());
	set_zero_if(val);
	set_negative_if(val);
}

/*****************
 * Jumps & Calls *
 *****************/
/// JMP - Jump to another location
void 
CPU::jmp(const uint16_t &addr) {
	// TODO: Original 6502 doesn't correctly fetch address.
	program_counter = addr;
}

/// JSR - Jump to a subroutine
void 
CPU::jsr(const uint16_t &addr) {
	memory.at(STACK_BASE - stack_pointer) = get_pch();
	stack_pointer++;
	memory.at(STACK_BASE - stack_pointer) = get_pcl();
	stack_pointer++;

	program_counter = addr;
}

/// RTS - Return from subroutine
void 
CPU::rts() {
	stack_pointer--;
	program_counter = uint16_t(memory.at(STACK_BASE - stack_pointer));
	stack_pointer--;
	program_counter |= uint16_t(memory.at(STACK_BASE - stack_pointer)) << 8;
	program_counter++;
}

/************
 * Branches *
 ************/
/// BCC - Branch if carry flag clear
void 
CPU::bcc(const int8_t &offset) {
	if (cpu_status[Carry] == 0) {
		program_counter += offset;
	}
}

/// BCS - Branch if carry flag set
void 
CPU::bcs(const int8_t &offset) {
	if (cpu_status[Carry] == 1) {
		program_counter += offset;
	}
}

/// BEQ - Branch if zero flag set
void 
CPU::beq(const int8_t &offset) {
	if (cpu_status[Zero] == 1) {
		program_counter += offset;
	}
}

/// BMI - Branch if negative flag set
void 
CPU::bmi(const int8_t &offset) {
	if (cpu_status[Negative] == 1) {
		program_counter += offset;
	}
}

/// BNE - Branch if zero flag clear
void 
CPU::bne(const int8_t &offset) {
	if (cpu_status[Zero] == 0) {
		program_counter += offset;
	}
}

/// BPL - Branch if negative flag clear
void 
CPU::bpl(const int8_t &offset) {
	if (cpu_status[Negative] == 0) {
		program_counter += offset;
	}
}

/// BVC - Branch if overflow flag clear
void 
CPU::bcv(const int8_t &offset) {
	if (cpu_status[Overflow] == 0) {
		program_counter += offset;
	}
}

/// BVS - Branch if overflow flag set
void 
CPU::bvs(const int8_t &offset) {
	if (cpu_status[Overflow] == 1) {
		program_counter += offset;
	}
}

/***********************
 * Status Flag Changes *
 ***********************/
/// CLC - Clear carry flag
void 
CPU::clc() {
	cpu_status.reset(Carry);
}

/// CLD - Clear decimal mode flag
void 
CPU::cld() {
	cpu_status.reset(DecimalMode);
}

/// CLI - Clear interrupt disable flag
void 
CPU::cli() {
	cpu_status.reset(InterruptDisable);
}

/// CLV - Clear overflow flag
void 
CPU::clv() {
	cpu_status.reset(Overflow);
}

/// SEC - Set carry flag
void 
CPU::sec() {
	cpu_status.set(Carry);
}

/// SED - Set decimal mode flag
void 
CPU::sed() {
	cpu_status.set(DecimalMode);
}

/// SEI - Set interrupt disable flag
void 
CPU::sei() {
	cpu_status.set(InterruptDisable);
}

/********************
 * System Functions *
 ********************/
/// BRK - Force an interrupt
void 
CPU::brk() {
	// TODO: FIX?
	program_counter =+ 2;
	uint8_t pcl = uint8_t(program_counter | 0);
	uint8_t pch = uint8_t((program_counter >> 8) | 0);

	memory.at(STACK_BASE - stack_pointer) = pcl;
	stack_pointer++;
	memory.at(STACK_BASE - stack_pointer) = pch;
	stack_pointer++;
	memory.at(STACK_BASE - stack_pointer) = uint8_t(cpu_status.to_ulong());
	stack_pointer++;

	cpu_status.set(BreakCommand);

	program_counter = 0;
	program_counter |= memory.at(0xffff);
	program_counter = program_counter << 8;
	program_counter |= memory.at(0xfffe);
}

/// NOP - No Operation
void 
CPU::nop() {
	program_counter++;
}

/// RTI - Return from Interrupt
void
CPU::rti() {
	// TODO: FIX?
	stack_pointer--;
	cpu_status = memory.at(STACK_BASE - stack_pointer);

	program_counter = 0;

	stack_pointer--;
	program_counter |= memory.at(STACK_BASE - stack_pointer);

	program_counter = program_counter << 8;

	stack_pointer--;
	program_counter |= memory.at(STACK_BASE - stack_pointer);
}
