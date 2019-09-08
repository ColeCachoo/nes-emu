// instructions.cpp
//
#include "cpu/cpu.h"
#include "nesutils.h"

namespace cpu {

/// STACK_BASE + stack_pointer gives next free location on stack.
constexpr uint16_t STACK_BASE = 0x0100;

/*----------------------------------------------------------------------------*/

void CPU::stack_push(const uint8_t val)
{
    ram[STACK_BASE + stack_pointer] = val;
    stack_pointer--;
}

constexpr uint8_t CPU::stack_pop()
{
    stack_pointer++;
    return ram[STACK_BASE + stack_pointer];
}

void CPU::set_zero_if(const uint8_t val)
{
    if (val == 0) {
        status = set_bit(status, ZERO);
    } else {
        status = clear_bit(status, ZERO);
    }
}

void CPU::set_negative_if(const uint8_t val)
{
    if (val & NEGATIVE) {
        status = set_bit(status, NEGATIVE);
    } else {
        status = clear_bit(status, NEGATIVE);
    }
}

/*----------------------------------------------------------------------------*/

// Instruction comments taken from:
// http://obelisk.me.uk/6502/reference.html
/*************************
 * Load/Store Operations *
 *************************/
void CPU::lda(const uint8_t val)
{
    accumulator = val;
    set_zero_if(accumulator);
    set_negative_if(accumulator);
}

void CPU::ldx(const uint8_t val)
{
    x_index = val;
    set_zero_if(x_index);
    set_negative_if(x_index);
}

void CPU::ldy(const uint8_t val)
{
    y_index = val;
    set_zero_if(y_index);
    set_negative_if(y_index);
}

void CPU::sta(const uint16_t addr)
{
    ram[addr] = accumulator;
}

void CPU::stx(const uint16_t addr)
{
    ram[addr] = x_index;
}

void CPU::sty(const uint16_t addr)
{
    ram[addr] = y_index;
}

/**********************
 * Register Transfers *
 **********************/
void CPU::tax()
{
    x_index = accumulator;
    set_zero_if(x_index);
    set_negative_if(x_index);
}

void CPU::tay()
{
    y_index = accumulator;
    set_zero_if(y_index);
    set_negative_if(y_index);
}

void CPU::txa()
{
    accumulator = x_index;
    set_zero_if(accumulator);
    set_negative_if(accumulator);
}

void CPU::tya()
{
    accumulator = y_index;
    set_zero_if(accumulator);
    set_negative_if(accumulator);
}

/********************
 * Stack Operations *
 ********************/
void CPU::tsx()
{
    x_index = stack_pointer;
    set_zero_if(x_index);
    set_negative_if(x_index);
}

void CPU::txs()
{
    stack_pointer = x_index;
}

void CPU::pha()
{
    stack_push(accumulator);
}

void CPU::php()
{
    status = set_bit(status, BREAK | EXPANSION);
    stack_push(status);
    status = clear_bit(status, BREAK);
}

void CPU::pla()
{
    accumulator = stack_pop();
    set_zero_if(accumulator);
    set_negative_if(accumulator);
}

void CPU::plp()
{
    status = stack_pop();
    status = clear_bit(status, BREAK);
    status = set_bit(status, EXPANSION);
}

/***********
 * Logical *
 ***********/
void CPU::logical_and(const uint8_t val)
{
    accumulator &= val;
    set_zero_if(accumulator);
    set_negative_if(accumulator);
}

void CPU::eor(const uint8_t val)
{
    accumulator ^= val;
    set_zero_if(accumulator);
    set_negative_if(accumulator);
}

void CPU::ora(const uint8_t val)
{
    accumulator |= val;
    set_zero_if(accumulator);
    set_negative_if(accumulator);
}

void CPU::bit(const uint8_t val)
{
    set_zero_if(accumulator & val);
    set_negative_if(val);
    if (val & OVERFLW) {
        status = set_bit(status, OVERFLW);
    } else {
        status = clear_bit(status, OVERFLW);
    }
}

/**************
 * Arithmetic *
 **************/
void CPU::adc(const uint8_t val)
{
    // Used later to check for overflow. Needs accumulator before it gets changed.
    uint8_t old_a = accumulator;

    uint8_t carry_bit;
    if (status & CARRY) {
        carry_bit = 0x01;
    } else {
        carry_bit = 0x00;
    }

    uint16_t result = accumulator + val + carry_bit;

    accumulator = (uint8_t) result;
    set_zero_if(accumulator);
    set_negative_if(accumulator);
    // Set carry if 9th position bit is set.
    if (result & 0x0100) {
        status = set_bit(status, CARRY);
    } else {
        status = clear_bit(status, CARRY);
    }

    // Check for overflow and set cpu_status if needed.
    // XOR original accumulator with value in memory, if this produces a 1 then no
    // overflow is possible. Else, they have the same bit in the 8th position, and
    // if the result is a different bit in that position, then overflow has
    // occurred.
    if ((old_a ^ val) & 0x80) {
        status = clear_bit(status, OVERFLW);
    } else if ((old_a & val & 0x80) == (result & 0x80)) {
        status = clear_bit(status, OVERFLW);
    } else {
        status = set_bit(status, OVERFLW);
    }
}

void CPU::sbc(const uint8_t val)
{
    // Used later to check for overflow. Needs accumulator before it gets changed.
    uint8_t old_a = accumulator;

    uint8_t carry_bit;
    if (status & CARRY) {
        carry_bit = 0x01;
    } else {
        carry_bit = 0x00;
    }

    // ~val needs to be casted or else the formula won't produce the carry bit in
    // the 9th bit position.
    uint16_t result = accumulator + ((uint8_t) ~val) + carry_bit;

    accumulator = (uint8_t) result;
    set_zero_if(accumulator);
    set_negative_if(accumulator);
    // Set carry if 9th position bit is set.
    if (result & 0x0100) {
        status = set_bit(status, CARRY);
    } else {
        status = clear_bit(status, CARRY);
    }

    // Check for overflow and set cpu_status if needed.
    // XOR original accumulator with value in memory's complement + carry bit, if
    // this produces a 1 then no overflow is possible. Else, they have the same bit
    // in the 8th position, and if the result is a different bit in that position,
    // then overflow has occurred.
    if ((old_a ^ (~val + carry_bit)) & 0x80) {
        status = clear_bit(status, OVERFLW);
    } else if ((old_a & (~val + carry_bit) & 0x80) == (result & 0x80)) {
        status = clear_bit(status, OVERFLW);
    } else {
        status = set_bit(status, OVERFLW);
    }
}

void CPU::compare(const uint8_t reg, const uint8_t val)
{
    if (reg >= val) {
        status = set_bit(status, CARRY);
    } else {
        status = clear_bit(status, CARRY);
    }

    if (reg == val) {
        status = set_bit(status, ZERO);
    } else {
        status = clear_bit(status, ZERO);
    }

    if ((reg - val) & NEGATIVE) {
        status = set_bit(status, NEGATIVE);
    } else {
        status = clear_bit(status, NEGATIVE);
    }
}

void CPU::cmp(const uint8_t val)
{
    compare(accumulator, val);
}

void CPU::cpx(const uint8_t val)
{
    compare(x_index, val);
}

void CPU::cpy(const uint8_t val)
{
    compare(y_index, val);
}

/***************************
 * Increments & Decrements *
 ***************************/
void CPU::inc(const uint16_t addr)
{
    ram[addr]++;
    set_zero_if(ram[addr]);
    set_negative_if(ram[addr]);
}

void CPU::inx()
{
    x_index++;
    set_zero_if(x_index);
    set_negative_if(x_index);
}

void CPU::iny()
{
    y_index++;
    set_zero_if(y_index);
    set_negative_if(y_index);
}

void CPU::dec(const uint16_t addr)
{
    ram[addr]--;
    set_zero_if(ram[addr]);
    set_negative_if(ram[addr]);
}

void CPU::dex()
{
    x_index--;
    set_zero_if(x_index);
    set_negative_if(x_index);
}

void CPU::dey()
{
    y_index--;
    set_zero_if(y_index);
    set_negative_if(y_index);
}

/**********
 * Shifts *
 **********/
void CPU::asl(uint8_t *val)
{
    if (*val & 0x80) {
        status = set_bit(status, CARRY);
    } else {
        status = clear_bit(status, CARRY);
    }

    *val <<= 1;

    set_zero_if(*val);
    set_negative_if(*val);
}

void CPU::lsr(uint8_t *val)
{
    if (*val & 0x01) {
        status = set_bit(status, CARRY);
    } else {
        status = clear_bit(status, CARRY);
    }

    *val >>= 1;

    set_zero_if(*val);
    set_negative_if(*val);
}

void CPU::rol(uint8_t *val)
{
    uint8_t new_carry = *val & 0x80;

    // Rotate left.
    *val = (*val >> 7) | (*val << 1);

    if (new_carry) {
        status = set_bit(status, CARRY);
    } else {
        status = clear_bit(status, CARRY);
    }

    set_zero_if(*val);
    set_negative_if(*val);
}

void CPU::ror(uint8_t *val)
{
    uint8_t new_carry = *val & 0x01;

    // Rotate right.
    *val = (status << 7) | (*val >> 1);

    if (new_carry) {
        status = set_bit(status, CARRY);
    } else {
        status = clear_bit(status, CARRY);
    }

    set_zero_if(*val);
    set_negative_if(*val);
}

/*****************
 * Jumps & Calls *
 *****************/
void CPU::jmp(const uint16_t addr)
{
    program_counter = addr;
}

void CPU::jsr(const uint16_t addr)
{
    program_counter--;
    stack_push(high_byte(program_counter));
    stack_push(low_byte(program_counter));
    program_counter = addr;
}

void CPU::rts()
{
    const uint16_t pcl = stack_pop();
    const uint16_t pch = ((uint8_t) stack_pop()) << 8;
    program_counter = (pch | pcl);
    program_counter++;
}

/************
 * Branches *
 ************/
void CPU::bcc()
{
    if (!(status & CARRY)) {
        int8_t offset = relative();
        program_counter += offset;
    } else {
        program_counter++;
    }
}

void CPU::bcs()
{
    if (status & CARRY) {
        int8_t offset = relative();
        program_counter += offset;
    } else {
        program_counter++;
    }
}

void CPU::beq()
{
    if (status & ZERO) {
        int8_t offset = relative();
        program_counter += offset;
    } else {
        program_counter++;
    }
}

void CPU::bmi()
{
    if (status & NEGATIVE) {
        int8_t offset = relative();
        program_counter += offset;
    } else {
        program_counter++;
    }
}

void CPU::bne()
{
    if (!(status & ZERO)) {
        int8_t offset = relative();
        program_counter += offset;
    } else {
        program_counter++;
    }
}

void CPU::bpl()
{
    if (!(status & NEGATIVE)) {
        int8_t offset = relative();
        program_counter += offset;
    } else {
        program_counter++;
    }
}

void CPU::bvc()
{
    if (!(status & OVERFLW)) {
        int8_t offset = relative();
        program_counter += offset;
    } else {
        program_counter++;
    }
}

void CPU::bvs()
{
    if (status & OVERFLW) {
        int8_t offset = relative();
        program_counter += offset;
    } else {
        program_counter++;
    }
}

/***********************
 * Status Flag Changes *
 ***********************/
void CPU::clc()
{
    status = clear_bit(status, CARRY);
}

void CPU::cld()
{
    status = clear_bit(status, DECIMAL);
}

void CPU::cli()
{
    status = clear_bit(status, INTERRUPT);
}

void CPU::clv()
{
    status = clear_bit(status, OVERFLW);
}

void CPU::sec()
{
    status = set_bit(status, CARRY);
}

void CPU::sed()
{
    status = set_bit(status, DECIMAL);
}

void CPU::sei()
{
    status = set_bit(status, INTERRUPT);
}

/********************
 * System Functions *
 ********************/
void CPU::brk()
{
    interrupt(BRK);
}

void CPU::nop()
{
    // Nothing as program counter has already been incremented.
}

void CPU::rti()
{
    status = stack_pop();
    status = set_bit(status, EXPANSION);
    const uint16_t pcl = stack_pop();
    const uint16_t pch = stack_pop() << 8;
    program_counter = (pch | pcl);
}

}   // Namespace cpu.