// instructions.cpp
//
#include "cpu/cpu.h"
#include "nes-utils.h"

namespace cpu {

/*----------------------------------------------------------------------------*/

void CPU::stack_push(uint8_t val)
{
    ram[STACK_BASE + stack_pointer] = val;
    stack_pointer--;
}

uint8_t CPU::stack_pop()
{
    stack_pointer++;
    return ram[STACK_BASE + stack_pointer];
}

void CPU::set_zero_if(uint8_t val)
{
    if (val == 0) {
        status = SET_BIT(status, ZERO);
    } else {
        status = CLEAR_BIT(status, ZERO);
    }
}

void CPU::set_negative_if(uint8_t val)
{
    if (val & NEGATIVE) {
        status = SET_BIT(status, NEGATIVE);
    } else {
        status = CLEAR_BIT(status, NEGATIVE);
    }
}

/*----------------------------------------------------------------------------*/

// Instruction comments taken from:
// http://obelisk.me.uk/6502/reference.html
/*************************
 * Load/Store Operations *
 *************************/
void CPU::lda(uint8_t val)
{
    accumulator = val;
    set_zero_if(accumulator);
    set_negative_if(accumulator);
}

void CPU::ldx(uint8_t val)
{
    x_index = val;
    set_zero_if(x_index);
    set_negative_if(x_index);
}

void CPU::ldy(uint8_t val)
{
    y_index = val;
    set_zero_if(y_index);
    set_negative_if(y_index);
}

void CPU::sta(uint16_t addr)
{
    ram[addr] = accumulator;
}

void CPU::stx(uint16_t addr)
{
    ram[addr] = x_index;
}

void CPU::sty(uint16_t addr)
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
    status = SET_BIT(status, BREAK | EXPANSION);
    stack_push(status);
    status = CLEAR_BIT(status, BREAK);
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
    status = CLEAR_BIT(status, BREAK);
    status = SET_BIT(status, EXPANSION);
}

/***********
 * Logical *
 ***********/
void CPU::logical_and(uint8_t val)
{
    accumulator &= val;
    set_zero_if(accumulator);
    set_negative_if(accumulator);
}

void CPU::eor(uint8_t val)
{
    accumulator ^= val;
    set_zero_if(accumulator);
    set_negative_if(accumulator);
}

void CPU::ora(uint8_t val)
{
    accumulator |= val;
    set_zero_if(accumulator);
    set_negative_if(accumulator);
}

void CPU::bit(uint8_t val)
{
    set_zero_if(accumulator & val);
    set_negative_if(val);
    if (val & OVERFLW) {
        status = SET_BIT(status, OVERFLW);
    } else {
        status = CLEAR_BIT(status, OVERFLW);
    }
}

/**************
 * Arithmetic *
 **************/
void CPU::adc(uint8_t val)
{
    // Used later to check for overflow. Needs accumulator before it gets changed.
    uint8_t old_a = accumulator;

    const uint8_t carry_bit = [this]() -> uint8_t {
        if (status & CARRY) {
            return 0x01;
        } else {
            return 0x00;
        }
    }();

    uint16_t result = accumulator + val + carry_bit;

    accumulator = uint8_t(result);
    set_zero_if(accumulator);
    set_negative_if(accumulator);
    // Set carry if 9th position bit is set.
    if (result & 0x0100) {
        status = SET_BIT(status, CARRY);
    } else {
        status = CLEAR_BIT(status, CARRY);
    }

    // Check for overflow and set cpu_status if needed.
    // XOR original accumulator with value in memory, if this produces a 1 then no
    // overflow is possible. Else, they have the same bit in the 8th position, and
    // if the result is a different bit in that position, then overflow has
    // occurred.
    if ((old_a ^ val) & 0x80) {
        status = CLEAR_BIT(status, OVERFLW);
    } else if ((old_a & val & 0x80) == (result & 0x80)) {
        status = CLEAR_BIT(status, OVERFLW);
    } else {
        status = SET_BIT(status, OVERFLW);
    }
}

void CPU::sbc(uint8_t val)
{
    // Used later to check for overflow. Needs accumulator before it gets changed.
    uint8_t old_a = accumulator;

    uint8_t carry_bit = [this]() -> uint8_t {
        if (status & CARRY) {
            return 0x01;
        } else {
            return 0x00;
        }
    }();

    // ~val needs to be casted or else the formula won't produce the carry bit in
    // the 9th bit position.
    uint16_t result = accumulator + (uint8_t(~val)) + carry_bit;

    accumulator = uint8_t(result);
    set_zero_if(accumulator);
    set_negative_if(accumulator);
    // Set carry if 9th position bit is set.
    if (result & 0x0100) {
        status = SET_BIT(status, CARRY);
    } else {
        status = CLEAR_BIT(status, CARRY);
    }

    // Check for overflow and set cpu_status if needed.
    // XOR original accumulator with value in memory's complement + carry bit, if
    // this produces a 1 then no overflow is possible. Else, they have the same bit
    // in the 8th position, and if the result is a different bit in that position,
    // then overflow has occurred.
    if ((old_a ^ (~val + carry_bit)) & 0x80) {
        status = CLEAR_BIT(status, OVERFLW);
    } else if ((old_a & (~val + carry_bit) & 0x80) == (result & 0x80)) {
        status = CLEAR_BIT(status, OVERFLW);
    } else {
        status = SET_BIT(status, OVERFLW);
    }
}

void CPU::compare(uint8_t reg, uint8_t val)
{
    if (reg >= val) {
        status = SET_BIT(status, CARRY);
    } else {
        status = CLEAR_BIT(status, CARRY);
    }

    if (reg == val) {
        status = SET_BIT(status, ZERO);
    } else {
        status = CLEAR_BIT(status, ZERO);
    }

    if ((reg - val) & NEGATIVE) {
        status = SET_BIT(status, NEGATIVE);
    } else {
        status = CLEAR_BIT(status, NEGATIVE);
    }
}

void CPU::cmp(uint8_t val)
{
    compare(accumulator, val);
}

void CPU::cpx(uint8_t val)
{
    compare(x_index, val);
}

void CPU::cpy(uint8_t val)
{
    compare(y_index, val);
}

/***************************
 * Increments & Decrements *
 ***************************/
void CPU::inc(uint16_t addr)
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

void CPU::dec(uint16_t addr)
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
        status = SET_BIT(status, CARRY);
    } else {
        status = CLEAR_BIT(status, CARRY);
    }

    *val <<= 1;

    set_zero_if(*val);
    set_negative_if(*val);
}

void CPU::lsr(uint8_t *val)
{
    if (*val & 0x01) {
        status = SET_BIT(status, CARRY);
    } else {
        status = CLEAR_BIT(status, CARRY);
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
        status = SET_BIT(status, CARRY);
    } else {
        status = CLEAR_BIT(status, CARRY);
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
        status = SET_BIT(status, CARRY);
    } else {
        status = CLEAR_BIT(status, CARRY);
    }

    set_zero_if(*val);
    set_negative_if(*val);
}

/*****************
 * Jumps & Calls *
 *****************/
void CPU::jmp(uint16_t addr)
{
    program_counter = addr;
}

void CPU::jsr(uint16_t addr)
{
    program_counter--;
    stack_push(high_byte(program_counter));
    stack_push(low_byte(program_counter));
    program_counter = addr;
}

void CPU::rts()
{
    const uint16_t pcl = stack_pop();
    const uint16_t pch = (uint8_t(stack_pop())) << 8;
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
    status = CLEAR_BIT(status, CARRY);
}

void CPU::cld()
{
    status = CLEAR_BIT(status, DECIMAL);
}

void CPU::cli()
{
    status = CLEAR_BIT(status, INTERRUPT);
}

void CPU::clv()
{
    status = CLEAR_BIT(status, OVERFLW);
}

void CPU::sec()
{
    status = SET_BIT(status, CARRY);
}

void CPU::sed()
{
    status = SET_BIT(status, DECIMAL);
}

void CPU::sei()
{
    status = SET_BIT(status, INTERRUPT);
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
    status = SET_BIT(status, EXPANSION);
    const uint16_t pcl = stack_pop();
    const uint16_t pch = stack_pop() << 8;
    program_counter = (pch | pcl);
}

}   // Namespace cpu.