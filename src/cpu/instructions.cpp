// instructions.cpp : File contains only the cpu instructions.
//
#include "cpu.h"

using namespace std;

/*************************
 * Load/Store Operations *
 *************************/
/// LDA - Load Accumulator
void 
CPU::lda(Addressing addr_mode) {
    uint16_t addr = fetch_with(addr_mode);
    uint8_t val;
    if (addr_mode == Addressing::Immediate) {
        val = uint8_t(addr);
    } else {
        val = ram->read(addr);
    }
    accumulator = val;
    set_zero_if(accumulator);
    set_negative_if(accumulator);
}

/// LDX - Load X Register
void
CPU::ldx(Addressing addr_mode) {
    uint16_t addr = fetch_with(addr_mode);
    uint8_t val;
    if (addr_mode == Addressing::Immediate) {
        val = uint8_t(addr);
    } else {
        val = ram->read(addr);
    }
    x_index = val;
    set_zero_if(x_index);
    set_negative_if(x_index);
}

/// LDY - Load Y Register
void
CPU::ldy(Addressing addr_mode) {
    uint16_t addr = fetch_with(addr_mode);
    uint8_t val;
    if (addr_mode == Addressing::Immediate) {
        val = uint8_t(addr);
    } else {
        val = ram->read(addr);
    }
    y_index = val;
    set_zero_if(y_index);
    set_negative_if(y_index);
}

/// STA - Store Accumulator
void 
CPU::sta(Addressing addr_mode) {
    uint16_t addr = fetch_with(addr_mode);
    ram->write(accumulator, addr);
}

/// STX - Store X Register
void 
CPU::stx(Addressing addr_mode) {
    uint16_t addr = fetch_with(addr_mode);
    ram->write(x_index, addr);
}

/// STY - Store Y Register
void 
CPU::sty(Addressing addr_mode) {
    uint16_t addr = fetch_with(addr_mode);
    ram->write(y_index, addr);
}

/**********************
 * Register Transfers *
 **********************/
/// TAX - Transfer Accumulator to X
void 
CPU::tax(Addressing addr_mode) {
    if (addr_mode != Addressing::Implicit) {
        cerr << "Unexpected addressing mode in TAX: " << int(addr_mode) << "\n";
    }
    x_index = accumulator;
    set_zero_if(x_index);
    set_negative_if(x_index);
}

/// TAY - Transfer Accumulator to Y
void 
CPU::tay(Addressing addr_mode) {
    if (addr_mode != Addressing::Implicit) {
        cerr << "Unexpected addressing mode in TAY: " << int(addr_mode) << "\n";
    }
    y_index = accumulator;
    set_zero_if(y_index);
    set_negative_if(y_index);
}

/// TXA - Transfer X to Accumulator
void 
CPU::txa(Addressing addr_mode) {
    if (addr_mode != Addressing::Implicit) {
        cerr << "Unexpected addressing mode in TXA: " << int(addr_mode) << "\n";
    }
    accumulator = x_index;
    set_zero_if(accumulator);
    set_negative_if(accumulator);
}

/// TYA - Transfer Y to Accumulator
void 
CPU::tya(Addressing addr_mode) {
    if (addr_mode != Addressing::Implicit) {
        cerr << "Unexpected addressing mode in TYA: " << int(addr_mode) << "\n";
    }
        accumulator = y_index;
        set_zero_if(accumulator);
        set_negative_if(accumulator);
}

/********************
 * Stack Operations *
 ********************/
/// TSX - Transfer stack pointer to X	
void 
CPU::tsx(Addressing addr_mode) {
    if (addr_mode != Addressing::Implicit) {
        cerr << "Unexpected addressing mode in TSX: " << int(addr_mode) << "\n";
    }
	x_index = stack_pointer;
    set_zero_if(x_index);
    set_negative_if(x_index);
}

/// TXS - Transfer X to stack pointer	
void 
CPU::txs(Addressing addr_mode) {
    if (addr_mode != Addressing::Implicit) {
        cerr << "Unexpected addressing mode in TXS: " << int(addr_mode) << "\n";
    }
	stack_pointer = x_index;
}

/// PHA - Push accumulator on stack	
void 
CPU::pha(Addressing addr_mode) {
    if (addr_mode != Addressing::Implicit) {
        cerr << "Unexpected addressing mode in PHA: " << int(addr_mode) << "\n";
    }
    stack_push(accumulator);
}

/// PHP - Push processor status on stack	
void 
CPU::php(Addressing addr_mode) {
    if (addr_mode != Addressing::Implicit) {
        cerr << "Unexpected addressing mode in PHP: " << int(addr_mode) << "\n";
    }
    cpu_status.set(BreakCommand);
    cpu_status.set(Expansion);
    stack_push(uint8_t(cpu_status.to_ulong()));
    cpu_status.reset(BreakCommand);
}

/// PLA - Pull accumulator from stack	
void 
CPU::pla(Addressing addr_mode) {
    if (addr_mode != Addressing::Implicit) {
        cerr << "Unexpected addressing mode in PLA: " << int(addr_mode) << "\n";
    }
    accumulator = stack_pop();
    set_zero_if(accumulator);
    set_negative_if(accumulator);
}

/// PLP - Pull processor status from stack	
void 
CPU::plp(Addressing addr_mode) {
    if (addr_mode != Addressing::Implicit) {
        cerr << "Unexpected addressing mode in PLP: " << int(addr_mode) << "\n";
    }
    cpu_status = stack_pop();
    cpu_status.reset(BreakCommand);
    cpu_status.set(Expansion);
}

/***********
 * Logical *
 ***********/
/// AND - Logical AND
void 
CPU::logical_and(Addressing addr_mode) {
    uint16_t addr = fetch_with(addr_mode);
    uint8_t val;
    if (addr_mode == Addressing::Immediate) {
        val = uint8_t(addr);
    } else {
        val = ram->read(addr);
    }
    accumulator &= val;
    set_zero_if(accumulator);
    set_negative_if(accumulator);
}

/// EOR - Exclusive OR
void 
CPU::eor(Addressing addr_mode) {
    uint16_t addr = fetch_with(addr_mode);
    uint8_t val;
    if (addr_mode == Addressing::Immediate) {
        val = uint8_t(addr);
    } else {
        val = ram->read(addr);
    }
    accumulator ^= val;
    set_zero_if(accumulator);
    set_negative_if(accumulator);
}

/// ORA - Logical Inclusive OR
void
CPU::ora(Addressing addr_mode) {
    uint16_t addr = fetch_with(addr_mode);
    uint8_t val;
    if (addr_mode == Addressing::Immediate) {
        val = uint8_t(addr);
    } else {
        val = ram->read(addr);
    }
    accumulator |= val;
    set_zero_if(accumulator);
    set_negative_if(accumulator);
}

/// BIT - Bit Test
void 
CPU::bit(Addressing addr_mode) {
    uint16_t addr = fetch_with(addr_mode);
    uint8_t val = ram->read(addr);
    set_zero_if(accumulator & val);
    set_negative_if(val);
    if (val & 0x40) {
        cpu_status.set(Overflow);
    } else {
        cpu_status.reset(Overflow);
    }
}

/**************
 * Arithmetic *
 **************/
/// ADC - Add with Carry
void 
CPU::adc(Addressing addr_mode) {
    uint16_t addr = fetch_with(addr_mode);
    uint8_t val;
    if (addr_mode == Addressing::Immediate) {
        val = uint8_t(addr);
    } else {
        val = ram->read(addr);
    }
    // Used later to check for overflow. Needs accumulator before it gets changed.
    uint8_t old_a = accumulator;

    uint8_t carry_bit;
    if (cpu_status[Carry] == 1) {
        carry_bit = 0x01;
    } else {
        carry_bit = 0x00;
    }

    uint16_t result = accumulator + val + carry_bit;

    accumulator = uint8_t(result);
    set_zero_if(accumulator);
    set_negative_if(accumulator);
    // Set carry if 9th position bit is set.
    if (result & 0x0100) {
        cpu_status.set(Carry);
    } else {
        cpu_status.reset(Carry);
    }

    // Check for overflow and set cpu_status if needed.
    // XOR original accumulator with value in memory, if this produces a 1 then no
    // overflow is possible. Else, they have the same bit in the 8th position, and
    // if the result is a different bit in that position, then overflow has
    // occurred.
    if ((old_a ^ val) & 0x80) {
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
    uint16_t addr = fetch_with(addr_mode);
    uint8_t val;
    if (addr_mode == Addressing::Immediate) {
        val = uint8_t(addr);
    } else {
        val = ram->read(addr);
    }
    // Used later to check for overflow. Needs accumulator before it gets changed.
    uint8_t old_a = accumulator;

    uint8_t carry_bit;
    if (cpu_status[Carry] == 1) {
        carry_bit = 0x01;
    } else {
        carry_bit = 0x00;
    }

    // ~val needs to be casted or else the formula won't produce the carry bit in
    // the 9th bit position.
    uint16_t result = accumulator + uint8_t(~val) + carry_bit;

    accumulator = uint8_t(result);
    set_zero_if(accumulator);
    set_negative_if(accumulator);
    // Set carry if 9th position bit is set.
    if (result & 0x0100) {
        cpu_status.set(Carry);
    } else {
        cpu_status.reset(Carry);
    }

    // Check for overflow and set cpu_status if needed.
    // XOR original accumulator with value in memory's complement + carry bit, if
    // this produces a 1 then no overflow is possible. Else, they have the same bit
    // in the 8th position, and if the result is a different bit in that position,
    // then overflow has occurred.
    if ((old_a ^ (~val + carry_bit)) & 0x80) {
        cpu_status.reset(Overflow);
    } else if ((old_a & (~val + carry_bit) & 0x80) == (result & 0x80)) {
        cpu_status.reset(Overflow);
    } else {
        cpu_status.set(Overflow);
    }
}

/// CMP - Compare
void 
CPU::cmp(Addressing addr_mode) {
    uint16_t addr = fetch_with(addr_mode);
    uint8_t val;
    if (addr_mode == Addressing::Immediate) {
        val = uint8_t(addr);
    } else {
        val = ram->read(addr);
    }
    if (accumulator >= val) {
        cpu_status.set(Carry);
    } else {
        cpu_status.reset(Carry);
    }

    if (accumulator == val) {
        cpu_status.set(Zero);
    } else {
        cpu_status.reset(Zero);
    }

    if ((accumulator - val) & 0x80) {
        cpu_status.set(Negative);
    } else {
        cpu_status.reset(Negative);
    }
}

/// CPX - Compare X Register
void 
CPU::cpx(Addressing addr_mode) {
    uint16_t addr = fetch_with(addr_mode);
    uint8_t val;
    if (addr_mode == Addressing::Immediate) {
        val = uint8_t(addr);
    } else {
        val = ram->read(addr);
    }
    if (x_index >= val) {
        cpu_status.set(Carry);
    } else {
        cpu_status.reset(Carry);
    }

    if (x_index == val) {
        cpu_status.set(Zero);
    } else {
        cpu_status.reset(Zero);
    }

    if ((x_index - val) & 0x80) {
        cpu_status.set(Negative);
    } else {
        cpu_status.reset(Negative);
    }
}

/// CPY - Compare Y Register
void 
CPU::cpy(Addressing addr_mode) {
    uint16_t addr = fetch_with(addr_mode);
    uint8_t val;
    if (addr_mode == Addressing::Immediate) {
        val = uint8_t(addr);
    } else {
        val = ram->read(addr);
    }

    if (y_index >= val) {
        cpu_status.set(Carry);
    } else {
        cpu_status.reset(Carry);
    }

    if (y_index == val) {
        cpu_status.set(Zero);
    } else {
        cpu_status.reset(Zero);
    }

    if ((y_index - val) & 0x80) {
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
CPU::inc(Addressing addr_mode) {
    uint16_t addr = fetch_with(addr_mode);
    uint8_t val = ram->read(addr);
    val++;
    ram->write(val, addr);
    set_zero_if(val);
    set_negative_if(val);
}

/// INX - Increment the X register
void 
CPU::inx(Addressing addr_mode) {
    if (addr_mode != Addressing::Implicit) {
        cerr << "Unexpected addressing mode in INX: " << int(addr_mode) << "\n";
    }
    x_index++;
    set_zero_if(x_index);
    set_negative_if(x_index);
}

/// INY - Increment the Y register
void 
CPU::iny(Addressing addr_mode) {
    if (addr_mode != Addressing::Implicit) {
        cerr << "Unexpected addressing mode in INY: " << int(addr_mode) << "\n";
    }
    y_index++;
    set_zero_if(y_index);
    set_negative_if(y_index);
}

/// DEC - Decrement a memory location
void 
CPU::dec(Addressing addr_mode) {
    uint16_t addr = fetch_with(addr_mode);
    uint8_t val = ram->read(addr);
    val--;
    ram->write(val, addr);
    set_zero_if(val);
    set_negative_if(val);
}

/// DEX - Decrement the X register
void 
CPU::dex(Addressing addr_mode) {
    if (addr_mode != Addressing::Implicit) {
        cerr << "Unexpected addressing mode in DEX: " << int(addr_mode) << "\n";
    }
    x_index--;
    set_zero_if(x_index);
    set_negative_if(x_index);
}

/// DEY - Decrement the Y register
void 
CPU::dey(Addressing addr_mode) {
    if (addr_mode != Addressing::Implicit) {
        cerr << "Unexpected addressing mode in DEY: " << int(addr_mode) << "\n";
    }
    y_index--;
    set_zero_if(y_index);
    set_negative_if(y_index);
}

/**********
 * Shifts *
 **********/
/// ASL - 	Arithmetic Shift Left
void 
CPU::asl(Addressing addr_mode) {
    uint16_t addr;
    uint8_t val;
    if (addr_mode == Addressing::Accumulator) {
        addr = 0;	// Not used.
        val = accumulator;
    } else {
        addr = fetch_with(addr_mode);
        val = ram->read(addr);
    }

    cpu_status.set(Carry, val & 0x80);
    val = val << 1;
    set_zero_if(val);
    set_negative_if(val);

    if (addr_mode == Addressing::Accumulator) {
        accumulator = val;
    } else {
        ram->write(val, addr);
    }
}

/// LSR - Logical Shift Right
void 
CPU::lsr(Addressing addr_mode) {
    uint16_t addr;
    uint8_t val;
    if (addr_mode == Addressing::Accumulator) {
        addr = 0;	// Not used.
        val = accumulator;
    } else {
        addr = fetch_with(addr_mode);
        val = ram->read(addr);
    }

    cpu_status.set(Carry, val & 0x01);
    val >>= 1;
    set_zero_if(val);
    set_negative_if(val);

    if (addr_mode == Addressing::Accumulator) {
        accumulator = val;
    } else {
        ram->write(val, addr);
    }
}

/// ROL - Rotate Left
void 
CPU::rol(Addressing addr_mode) {
    uint16_t addr;
    uint8_t val;
    if (addr_mode == Addressing::Accumulator) {
        addr = 0;	// Not used.
        val = accumulator;
    } else {
        addr = fetch_with(addr_mode);
        val = ram->read(addr);
    }

    bitset<9> v = val;
    v = v << 1;
    v[0] = cpu_status[Carry];
    cpu_status.set(Carry, v[8]);

    val = uint8_t(v.to_ulong());
    set_zero_if(val);
    set_negative_if(val);

    if (addr_mode == Addressing::Accumulator) {
        accumulator = val;
    } else {
        ram->write(val, addr);
    }
}

/// ROR - Rotate Right
void 
CPU::ror(Addressing addr_mode) {
    uint16_t addr;
    uint8_t val;
    if (addr_mode == Addressing::Accumulator) {
        addr = 0;	// Not used.
        val = accumulator;
    } else {
        addr = fetch_with(addr_mode);
        val = ram->read(addr);
    }

    bitset<9> v = val;
    v[8] = cpu_status[Carry];
    cpu_status.set(Carry, v[0]);
    v = v >> 1;

    val = uint8_t(v.to_ulong());
    set_zero_if(val);
    set_negative_if(val);

    if (addr_mode == Addressing::Accumulator) {
        accumulator = val;
    } else {
        ram->write(val, addr);
    }
}

/*****************
 * Jumps & Calls *
 *****************/
/// JMP - Jump to another location
void 
CPU::jmp(Addressing addr_mode) {
    uint16_t addr = fetch_with(addr_mode);
    program_counter = addr;
}

/// JSR - Jump to a subroutine
void 
CPU::jsr(Addressing addr_mode) {
    uint16_t addr = fetch_with(addr_mode);
    program_counter--;
    stack_push(get_pch());
    stack_push(get_pcl());
    program_counter = addr;
}

/// RTS - Return from subroutine
void 
CPU::rts(Addressing addr_mode) {
    if (addr_mode != Addressing::Implicit) {
        cerr << "Unexpected addressing mode in RTS: " << int(addr_mode) << "\n";
    }
    uint16_t pcl = stack_pop();
    uint16_t pch = stack_pop();
    pch <<= 8;
    program_counter = (pch | pcl);
    program_counter++;
}

/************
 * Branches *
 ************/
/// BCC - Branch if carry flag clear
void 
CPU::bcc(Addressing addr_mode) {
    if (cpu_status[Carry] == 0) {
        int8_t offset = fetch_with(addr_mode);
        program_counter += offset;
    } else {
        program_counter++;
    }
}

/// BCS - Branch if carry flag set
void 
CPU::bcs(Addressing addr_mode) {
    if (cpu_status[Carry] == 1) {
        int8_t offset = fetch_with(addr_mode);
        program_counter += offset;
    } else {
        program_counter++;
    }
}

/// BEQ - Branch if zero flag set
void 
CPU::beq(Addressing addr_mode) {
    if (cpu_status[Zero] == 1) {
        int8_t offset = fetch_with(addr_mode);
        program_counter += offset;
    } else {
        program_counter++;
    }
}

/// BMI - Branch if negative flag set
void 
CPU::bmi(Addressing addr_mode) {
    if (cpu_status[Negative] == 1) {
        int8_t offset = fetch_with(addr_mode);
        program_counter += offset;
    } else {
        program_counter++;
    }
}

/// BNE - Branch if zero flag clear
void 
CPU::bne(Addressing addr_mode) {
    if (cpu_status[Zero] == 0) {
        int8_t offset = fetch_with(addr_mode);
        program_counter += offset;
    } else {
        program_counter++;
    }
}

/// BPL - Branch if negative flag clear
void 
CPU::bpl(Addressing addr_mode) {
    if (cpu_status[Negative] == 0) {
        int8_t offset = fetch_with(addr_mode);
        program_counter += offset;
    } else {
        program_counter++;
    }
}

/// BVC - Branch if overflow flag clear
void 
CPU::bvc(Addressing addr_mode) {
    if (cpu_status[Overflow] == 0) {
        int8_t offset = fetch_with(addr_mode);
        program_counter += offset;
    } else {
        program_counter++;
    }
}

/// BVS - Branch if overflow flag set
void 
CPU::bvs(Addressing addr_mode) {
    if (cpu_status[Overflow] == 1) {
        int8_t offset = fetch_with(addr_mode);
        program_counter += offset;
    } else {
        program_counter++;
    }
}

/***********************
 * Status Flag Changes *
 ***********************/
/// CLC - Clear carry flag
void 
CPU::clc(Addressing addr_mode) {
    if (addr_mode != Addressing::Implicit) {
        cerr << "Unexpected addressing mode in CLC: " << int(addr_mode) << "\n";
    }
    cpu_status.reset(Carry);
}

/// CLD - Clear decimal mode flag
void 
CPU::cld(Addressing addr_mode) {
    if (addr_mode != Addressing::Implicit) {
        cerr << "Unexpected addressing mode in CLD: " << int(addr_mode) << "\n";
    }
    cpu_status.reset(DecimalMode);
}

/// CLI - Clear interrupt disable flag
void 
CPU::cli(Addressing addr_mode) {
    if (addr_mode != Addressing::Implicit) {
        cerr << "Unexpected addressing mode in CLI: " << int(addr_mode) << "\n";
    }
    cpu_status.reset(InterruptDisable);
}

/// CLV - Clear overflow flag
void 
CPU::clv(Addressing addr_mode) {
    if (addr_mode != Addressing::Implicit) {
        cerr << "Unexpected addressing mode in CLV: " << int(addr_mode) << "\n";
    }
    cpu_status.reset(Overflow);
}

/// SEC - Set carry flag
void 
CPU::sec(Addressing addr_mode) {
    if (addr_mode != Addressing::Implicit) {
        cerr << "Unexpected addressing mode in SEC: " << int(addr_mode) << "\n";
    }
    cpu_status.set(Carry);
}

/// SED - Set decimal mode flag
void 
CPU::sed(Addressing addr_mode) {
    if (addr_mode != Addressing::Implicit) {
        cerr << "Unexpected addressing mode in SED: " << int(addr_mode) << "\n";
    }
    cpu_status.set(DecimalMode);
}

/// SEI - Set interrupt disable flag
void 
CPU::sei(Addressing addr_mode) {
    if (addr_mode != Addressing::Implicit) {
        cerr << "Unexpected addressing mode in SEI: " << int(addr_mode) << "\n";
    }
    cpu_status.set(InterruptDisable);
}

/********************
 * System Functions *
 ********************/
/// BRK - Force an interrupt
void 
CPU::brk(Addressing addr_mode) {
    if (addr_mode != Addressing::Implicit) {
        cerr << "Unexpected addressing mode in BRK: " << int(addr_mode) << "\n";
    }
    // TODO: Is this how much I should increment PC?
    program_counter += 2;
    stack_push(get_pch());
    stack_push(get_pcl());
    cpu_status.set(BreakCommand);
    cpu_status.set(Expansion);
    stack_push(uint8_t(cpu_status.to_ulong()));
    cpu_status.set(InterruptDisable);

    uint16_t vec_low  = uint16_t(ram->read(0xfffe)) << 8;
    uint16_t vec_high = uint16_t(ram->read(0xffff));
    program_counter = (vec_high | vec_low);
}

/// NOP - No Operation
void 
CPU::nop(Addressing addr_mode) {
    switch (addr_mode) {
    case(Addressing::Implicit):
        // Nothing.
        break;
    case(Addressing::Immediate):
        fetch_with(Addressing::Immediate);
        break;
    case(Addressing::ZeroPage):
        fetch_with(Addressing::ZeroPage);
        break;
    case(Addressing::ZeroPageX):
        fetch_with(Addressing::ZeroPageX);
        break;
    case(Addressing::Absolute):
        fetch_with(Addressing::Absolute);
        break;
    case(Addressing::AbsoluteX):
        fetch_with(Addressing::AbsoluteX);
        break;
    default:
        cerr << "Unexpected addressing mode in NOP: " << int(addr_mode) << "\n";
    }
}

/// RTI - Return from Interrupt
void
CPU::rti(Addressing addr_mode) {
    if (addr_mode != Addressing::Implicit) {
        cerr << "Unexpected addressing mode in RTI: " << int(addr_mode) << "\n";
    }
    cpu_status = stack_pop();
    cpu_status.set(Expansion);
    uint16_t pcl = stack_pop();
    uint16_t pch = stack_pop() << 8;
    program_counter = (pch | pcl);
}
