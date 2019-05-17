// cpu.h : Emulates the actions of the 6502 CPU in the NES.
//
#pragma once

#include <bitset>
#include <cstdint>
#include <functional>
#include <iostream>
#include <unordered_map>

// TODO: Remove these headers.
#include <vector>
#include <stdio.h>

#include "../ram/ram.h"

/***************************************************
	 _______________
	|N|V| |B|D|I|Z|C| -- Processor Status Register
	 | | | | | | | |
	 | | | | | | | +---- Carry
	 | | | | | | +------ Zero Result
	 | | | | | +-------- Interrupt Disable
	 | | | | +---------- Decimal Mode
	 | | | +------------ Break Command
	 | | +-------------- Expansion
	 | +---------------- Overflow
	 +------------------ Negative Result
****************************************************/
enum Status {
	// 0-7 represents bit position for std::bitset.
	Carry				= 0,	
	Zero				= 1,
	InterruptDisable	= 2,
	DecimalMode			= 3,
	BreakCommand		= 4,
	Expansion			= 5,	// Not used.
	Overflow			= 6,
	Negative	 		= 7,
};

/// Possible addressing modes.
enum class Addressing {
	Implicit,
	Accumulator,
	Immediate,
	ZeroPage,
	ZeroPageX,
	ZeroPageY,
	Relative,
	Absolute,
	AbsoluteX,
	AbsoluteY,
	Indirect,
	IndexedIndirect,
	IndirectIndexed,
};

class RAM;

class CPU {
public:
	// TODO: Force default constructor to throw an error.
	CPU() = default;
	CPU(RAM *ram);
	~CPU() = default;

	std::bitset<8> get_cpu_status() const { return cpu_status; }

	/*************************
	 * Load/Store Operations *
	 *************************/
	/// LDA - Load Accumulator
	/// Loads a byte of memory into the accumulator setting the zero and
	/// negative flags as appropriate.
	void lda(Addressing addr_mode);

	/// LDX - Load X Register
	/// Loads a byte of memory into the X register setting the zero and
	/// negative flags as appropriate.
	void ldx(Addressing addr_mode);

	/// LDY - Load Y Register
	/// Loads a byte of memory into the Y register setting the zero and
	/// negative flags as appropriate.
	void ldy(Addressing addr_mode);

	/// STA - Store Accumulator
	/// Stores the contents of the accumulator into memory.
	void sta(Addressing addr_mode);

	/// STX - Store X Register
	/// Stores the contents of the X register into memory.
	void stx(Addressing addr_mode);

	/// STY - Store Y Register
	/// Stores the contents of the Y register into memory.
	void sty(Addressing addr_mode);

	/**********************
	 * Register Transfers *
	 **********************/
	/// TAX - Transfer Accumulator to X
	/// Copies the current contents of the accumulator into the X register and
	/// sets the zero and negative flags as appropriate.
	void tax(Addressing addr_mode);

	/// TAY - Transfer Accumulator to Y
	/// Copies the current contents of the accumulator into the Y register and
	/// sets the zero and negative flags as appropriate.
	void tay(Addressing addr_mode);

	/// TXA - Transfer X to Accumulator
	/// Copies the current contents of the X register into the accumulator and
	/// sets the zero and negative flags as appropriate.
	void txa(Addressing addr_mode);

	/// TYA - Transfer Y to Accumulator
	/// Copies the current contents of the Y register into the accumulator and
	/// sets the zero and negative flags as appropriate.
	void tya(Addressing addr_mode);

	/********************
	 * Stack Operations *
	 ********************/
	/// TSX - Transfer stack pointer to X	
	/// Copies the current contents of the stack register into the X register
	/// and sets the zero and negative flags as appropriate.
	void tsx(Addressing addr_mode);

	/// TXS - Transfer X to stack pointer	
	/// Copies the current contents of the X register into the stack register.
	void txs(Addressing addr_mode);

	/// PHA - Push accumulator on stack	
	/// Pushes a copy of the accumulator on to the stack.
	void pha(Addressing addr_mode);

	/// PHP - Push processor status on stack	
	/// Pushes a copy of the status flags on to the stack.
	void php(Addressing addr_mode);

	/// PLA - Pull accumulator from stack	
	/// Pulls an 8 bit value from the stack and into the accumulator. The zero
	/// and negative flags are set as appropriate.
	void pla(Addressing addr_mode);

	/// PLP - Pull processor status from stack	
	/// Pulls an 8 bit value from the stack and into the processor flags. The
	/// flags will take on new states as determined by the value pulled.
	void plp(Addressing addr_mode);

	/***********
	 * Logical *
	 ***********/
	/// AND - Logical AND
	/// A logical AND is performed, bit by bit, on the accumulator contents
	/// using the contents of a byte of memory.
	void logical_and(Addressing addr_mode);

	/// EOR - Exclusive OR
	/// An exclusive OR is performed, bit by bit, on the accumulator contents
	/// using the contents of a byte of memory.
	void eor(Addressing addr_mode);

	/// ORA - Logical Inclusive OR
	/// An inclusive OR is performed, bit by bit, on the accumulator contents
	/// using the contents of a byte of memory.
	void ora(Addressing addr_mode);

	/// BIT - Bit Test
	/// This instructions is used to test if one or more bits are set in a
	/// target memory location. The mask pattern in A is ANDed with the value
	/// in memory to set or clear the zero flag, but the result is not kept.
	/// Bits 7 and 6 of the value from memory are copied into the N and V
	/// flags.
	void bit(Addressing addr_mode);

	/**************
	 * Arithmetic *
	 **************/
	/// ADC - Add with Carry
	/// This instruction adds the contents of a memory location to the
	/// accumulator together with the carry bit. If overflow occurs the carry
	/// bit is set, this enables multiple byte addition to be performed.
	void adc(Addressing addr_mode);

	/// SBC - Subtract with Carry
	/// This instruction subtracts the contents of a memory location to the
	/// accumulator together with the not of the carry bit. If overflow occurs
	/// the carry bit is clear, this enables multiple byte subtraction to be
	/// performed.
	void sbc(Addressing addr_mode);

	/// CMP - Compare
	/// This instruction compares the contents of the accumulator with another
	/// memory held value and sets the zero and carry flags as appropriate.
	void cmp(Addressing addr_mode);

	/// CPX - Compare X Register
	/// This instruction compares the contents of the X register with another
	/// memory held value and sets the zero and carry flags as appropriate.
	void cpx(Addressing addr_mode);

	/// CPY - Compare Y Register
	/// This instruction compares the contents of the Y register with another
	/// memory held value and sets the zero and carry flags as appropriate.
	void cpy(Addressing addr_mode);

	/***************************
	 * Increments & Decrements *
	 ***************************/
	/// INC - Increment a memory location
	/// Adds one to the value held at a specified memory location setting the
	/// zero and negative flags as appropriate.
	void inc(Addressing addr_mode);

	/// INX - Increment the X register
	/// Adds one to the X register setting the zero and negative flags as
	/// appropriate.
	void inx(Addressing addr_mode);

	/// INY - Increment the Y register
	/// Adds one to the Y register setting the zero and negative flags as
	/// appropriate.
	void iny(Addressing addr_mode);

	/// DEC - Decrement a memory location
	/// Subtracts one from the value held at a specified memory location
	/// setting the zero and negative flags as appropriate.
	void dec(Addressing addr_mode);

	/// DEX - Decrement the X register
	/// Subtracts one from the X register setting the zero and negative flags
	/// as appropriate.
	void dex(Addressing addr_mode);

	/// DEY - Decrement the Y register
	/// Subtracts one from the Y register setting the zero and negative flags
	/// as appropriate.
	void dey(Addressing addr_mode);

	/**********
	 * Shifts *
	 **********/
	/// ASL - 	Arithmetic Shift Left
	/// This operation shifts all the bits of the accumulator or memory
	/// contents one bit left. Bit 0 is set to 0 and bit 7 is placed in the
	/// carry flag. The effect of this operation is to multiply the memory
	/// contents by 2 (ignoring 2's complement considerations), setting the
	/// carry if the result will not fit in 8 bits.
	void asl(Addressing addr_mode);

	/// LSR - Logical Shift Right
	/// Each of the bits in A or M is shift one place to the right. The bit
	/// that was in bit 0 is shifted into the carry flag. Bit 7 is set to zero.
	void lsr(Addressing addr_mode);

	/// ROL - Rotate Left
	/// Move each of the bits in either A or M one place to the left. Bit 0 is
	/// filled with the current value of the carry flag whilst the old bit 7
	/// becomes the new carry flag value.
	void rol(Addressing addr_mode);

	/// ROR - Rotate Right
	/// Move each of the bits in either A or M one place to the right. Bit 7 is
	/// filled with the current value of the carry flag whilst the old bit 0
	/// becomes the new carry flag value.
	void ror(Addressing addr_mode);

	/*****************
	 * Jumps & Calls *
	 *****************/
	/// JMP - Jump to another location
	/// Sets the program counter to the address specified by the operand.
	void jmp(Addressing addr_mode);

	/// JSR - Jump to a subroutine
	/// The JSR instruction pushes the address (minus one) of the return point
	/// on to the stack and then sets the program counter to the target memory
	/// address.
	void jsr(Addressing addr_mode);

	/// RTS - Return from subroutine
	/// The RTS instruction is used at the end of a subroutine to return to the
	/// calling routine. It pulls the program counter (minus one) from the
	/// stack.
	void rts(Addressing addr_mode);

	/************
	 * Branches *
	 ************/
	/// BCC - Branch if carry flag clear
	/// If the carry flag is clear then add the relative displacement to the
	/// program counter to cause a branch to a new location.
	void bcc(Addressing addr_mode);

	/// BCS - Branch if carry flag set
	/// If the carry flag is set then add the offset displacement to the
	/// program counter to cause a branch to a new location.
	void bcs(Addressing addr_mode);

	/// BEQ - Branch if zero flag set
	/// If the zero flag is set then add the offset displacement to the
	/// program counter to cause a branch to a new location.
	void beq(Addressing addr_mode);

	/// BMI - Branch if negative flag set
	/// If the negative flag is set then add the offset displacement to the
	/// program counter to cause a branch to a new location.
	void bmi(Addressing addr_mode);
	
	/// BNE - Branch if zero flag clear
	/// If the zero flag is clear then add the offset displacement to the
	/// program counter to cause a branch to a new location.
	void bne(Addressing addr_mode);

	/// BPL - Branch if negative flag clear
	/// If the negative flag is clear then add the offset displacement to the
	/// program counter to cause a branch to a new location.
	void bpl(Addressing addr_mode);

	/// BVC - Branch if overflow flag clear
	/// If the overflow flag is clear then add the offset displacement to the
	/// program counter to cause a branch to a new location.
	void bvc(Addressing addr_mode);

	/// BVS - Branch if overflow flag set
	/// If the overflow flag is set then add the offset displacement to the
	/// program counter to cause a branch to a new location.
	void bvs(Addressing addr_mode);

	/***********************
	 * Status Flag Changes *
	 ***********************/
	/// CLC - Clear carry flag
	/// Set the carry flag to zero.
	void clc(Addressing addr_mode);

	/// CLD - Clear decimal mode flag
	/// Sets the decimal mode flag to zero.
	void cld(Addressing addr_mode);

	/// CLI - Clear interrupt disable flag
	/// Clears the interrupt disable flag allowing normal interrupt requests to
	/// be serviced.
	void cli(Addressing addr_mode);

	/// CLV - Clear overflow flag
	/// Clears the overflow flag.
	void clv(Addressing addr_mode);

	/// SEC - Set carry flag
	/// Set the carry flag to one.
	void sec(Addressing addr_mode);

	/// SED - Set decimal mode flag
	/// Set the decimal mode flag to one.
	void sed(Addressing addr_mode);

	/// SEI - Set interrupt disable flag
	/// Set the interrupt disable flag to one.
	void sei(Addressing addr_mode);

	/********************
	 * System Functions *
	 ********************/
	/// BRK - Force an interrupt
	/// The BRK instruction forces the generation of an interrupt request. The
	/// program counter and processor status are pushed on the stack then the
	/// IRQ interrupt vector at $FFFE/F is loaded into the PC and the break
	/// flag in the status set to one.
	void brk();

	/// NOP - No Operation
	/// The NOP instruction causes no changes to the processor other than the
	/// normal incrementing of the program counter to the next instruction.
	void nop();

	/// RTI - Return from Interrupt
	/// The RTI instruction is used at the end of an interrupt processing
	/// routine. It pulls the processor flags from the stack followed by the
	/// program counter.
	void rti();

private:
	// TODO: Explain my reasoning for stack_base and stack_pointer being how ther are.
	// Something like, this way when you push you're adding to the stack pointer,
	// and when pulling you're subtracting from the stack pointer.
	// (STACK_BASE - stack_pointer) gives next free memory location in stack.
	const uint16_t STACK_BASE = 0x01ff;

	// Used together with unordered_map to store all information about the
	// opcodes.
	struct OpCode {
		Addressing addr_mode;
		size_t bytes;	// Number of bytes addressing mode needs.
		size_t cycles;	// Number of cycles.
		std::function<void (Addressing)> op_func; // What instruction to call.
	};

	// Registers
	uint16_t program_counter;
	uint8_t stack_pointer;
	uint8_t accumulator;
	uint8_t x_index;
	uint8_t y_index;
	std::bitset<8> cpu_status;

	// 64Kb of memory.
	std::vector<uint8_t> memory;
	RAM *ram;

	/// Pushes data to stack.
	void stack_push(uint8_t val);
	/// Pops data from stack and returns it.
	uint8_t stack_pop();

	/// Set zero flag if given value == 0.
	void set_zero_if(uint8_t val);
	/// Set negative flag if bit 7 of val is set.
	void set_negative_if(uint8_t val);

	/// Returns lower 8 bits from program counter.
	uint8_t get_pcl() const { return uint8_t(program_counter | 0); }
	/// Returns higher 8 bits from program counter.
	uint8_t get_pch() const { return uint8_t(program_counter >> 8 | 0); }

	void data_bus();

	/// Get data from the memory address that is in program_counter,
	/// then increments the program_counter.
	uint8_t fetch();
	/// Uses Addressing mode to get address that can be used to get a value from memory.
	uint16_t fetch_with(Addressing addr_mode);

	/// Addressing mode functions.
	/// Returns the value at the address that it computes.
	uint16_t immediate();
	uint16_t zero_page();
	uint16_t zero_page_x();
	uint16_t zero_page_y();
	int8_t relative();
	uint16_t absolute();
	uint16_t absolute_x();
	uint16_t absolute_y();
	uint16_t indirect();
	uint16_t indexed_indirect();
	uint16_t indirect_indexed();
};

