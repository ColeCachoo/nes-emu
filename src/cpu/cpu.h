// cpu.h
//
#pragma once

#include "nes-error.h"

#include <cstdint>
#include <fstream>

namespace cpu {

enum Interrupt {
    BRK,
    IRQ,
    NMI,
    RESET,
};

class CPU {
public:
    CPU(uint8_t *mem);
    // CPU(CPU&) = default;
    // CPU(const CPU&) = default;
    ~CPU() = default;

    /// Returns value at address given by program counter.
    inline uint8_t fetch() { return ram[program_counter++]; }
    /// Executes given opcode.
    /// Returns ERROR if given an unknown opcode.
    NESerror execute(uint8_t opcode);

    /// Triggers interrupt the given interrupt.
    void interrupt(Interrupt interr);

    // TODO: Delete.
    NESerror run();

    // TODO: Tell the format.
    /// Prints CPU registers.
    void print() const;

    /// Same as print() but to a file.
    void fprint(std::ofstream &file) const;

private:
    /***************************************************
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
    // CPU status flags.
    static constexpr uint8_t CARRY       = 1 << 0;
    static constexpr uint8_t ZERO        = 1 << 1;
    static constexpr uint8_t INTERRUPT   = 1 << 2;
    static constexpr uint8_t DECIMAL     = 1 << 3;
    static constexpr uint8_t BREAK       = 1 << 4;
    static constexpr uint8_t EXPANSION   = 1 << 5;
    static constexpr uint8_t OVERFLW     = 1 << 6;
    static constexpr uint8_t NEGATIVE    = 1 << 7;

    /// STACK_BASE + stack_pointer gives next free location on stack.
    static constexpr uint16_t STACK_BASE = 0x0100;

    // Registers
    uint16_t program_counter;
    uint8_t stack_pointer;
    uint8_t accumulator;
    uint8_t x_index;
    uint8_t y_index;
    uint8_t status;

    // Shared memory.
    uint8_t *ram;

/*----------------------------------------------------------------------------*/

    /******************************
     * Addressing mode functions. *
     ******************************/
    // Returns address or value that is computed by chosen address mode.
    //
    inline int8_t  relative()            { return int8_t(fetch()); }
    inline uint8_t immediate()           { return fetch(); }
    inline uint8_t *get_accumulator()    { return &accumulator; }
    inline uint16_t zero_page()          { return fetch(); }
    inline uint16_t zero_page_x()        { return (x_index + fetch()) % 256; }
    inline uint16_t zero_page_y()        { return (y_index + fetch()) % 256; }

    inline uint16_t absolute()          
    {
        const uint16_t addr_low  = fetch();
        const uint16_t addr_high = fetch() << 8;
        return (addr_high | addr_low);
    }

    inline uint16_t absolute_x()        
    {
        const uint16_t addr_low  = fetch();
        const uint16_t addr_high = fetch() << 8;
        return (addr_high | addr_low) + x_index;
    }

    inline uint16_t absolute_y()        
    {
        const uint16_t addr_low  = fetch();
        const uint16_t addr_high = fetch() << 8;
        return (addr_high | addr_low) + y_index;
    }

    inline uint16_t indirect()
    {
        const uint16_t addr_low  = fetch();
        const uint16_t addr_high = fetch() << 8;
        const uint16_t new_low = ram[addr_high | addr_low];
        // An original 6502 has does not correctly fetch the target address if the
        // indirect vector falls on a page boundary (e.g. $xxFF where xx is any value
        // from $00 to $FF). In this case fetches the LSB from $xxFF as expected but
        // takes the MSB from $xx00.
        const uint16_t new_high = ram[addr_high | uint8_t((addr_low + 1))] << 8;
        return (new_high | new_low);
    }

    inline uint16_t indexed_indirect()  
    {
        const uint8_t val = fetch();
        return ram[(val + x_index) % 256] + ram[(val + x_index + 1) % 256] * 256;
    }

    inline uint16_t indirect_indexed()  
    {
        const uint8_t val = fetch();
        return ram[val] + ram[(val + 1) % 256] * 256 + y_index;
    }

/*----------------------------------------------------------------------------*/

    /// Pushes value to stack.
    void stack_push(uint8_t val);
    /// Pops value from stack and returns the value.
    uint8_t stack_pop();

    /// Set zero flag if given value equals 0, otherwise clear it.
    void set_zero_if(uint8_t val);
    /// Set negative flag if bit 7 of val is set, otherwise clear it.
    void set_negative_if(uint8_t val);

/*----------------------------------------------------------------------------*/

    // Instruction comments taken from:
    // http://obelisk.me.uk/6502/reference.html
    /*************************
     * Load/Store Operations *
     *************************/
    /// LDA - Load Accumulator
    /// Loads a byte of memory into the accumulator setting the zero and
    /// negative flags as appropriate.
    void lda(uint8_t val);
    /// LDX - Load X Register
    /// Loads a byte of memory into the X register setting the zero and
    /// negative flags as appropriate.
    void ldx(uint8_t val);
    /// LDY - Load Y Register
    /// Loads a byte of memory into the Y register setting the zero and
    /// negative flags as appropriate.
    void ldy(uint8_t val);
    /// STA - Store Accumulator
    /// Stores the contents of the accumulator into memory.
    void sta(uint16_t addr);
    /// STX - Store X Register
    /// Stores the contents of the X register into memory.
    void stx(uint16_t addr);
    /// STY - Store Y Register
    /// Stores the contents of the Y register into memory.
    void sty(uint16_t addr);

    /**********************
     * Register Transfers *
     **********************/
    /// TAX - Transfer Accumulator to X
    /// Copies the current contents of the accumulator into the X register and
    /// sets the zero and negative flags as appropriate.
    void tax();
    /// TAY - Transfer Accumulator to Y
    /// Copies the current contents of the accumulator into the Y register and
    /// sets the zero and negative flags as appropriate.
    void tay();
    /// TXA - Transfer X to Accumulator
    /// Copies the current contents of the X register into the accumulator and
    /// sets the zero and negative flags as appropriate.
    void txa();
    /// TYA - Transfer Y to Accumulator
    /// Copies the current contents of the Y register into the accumulator and
    /// sets the zero and negative flags as appropriate.
    void tya();

    /********************
     * Stack Operations *
     ********************/
    /// TSX - Transfer stack pointer to X	
    /// Copies the current contents of the stack register into the X register
    /// and sets the zero and negative flags as appropriate.
    void tsx();
    /// TXS - Transfer X to stack pointer	
    /// Copies the current contents of the X register into the stack register.
    void txs();
    /// PHA - Push accumulator on stack	
    /// Pushes a copy of the accumulator on to the stack.
    void pha();
    /// PHP - Push processor status on stack	
    /// Pushes a copy of the status flags on to the stack.
    void php();
    /// PLA - Pull accumulator from stack	
    /// Pulls an 8 bit value from the stack and into the accumulator. The zero
    /// and negative flags are set as appropriate.
    void pla();
    /// PLP - Pull processor status from stack	
    /// Pulls an 8 bit value from the stack and into the processor flags. The
    /// flags will take on new states as determined by the value pulled.
    void plp();

    /***********
     * Logical *
     ***********/
    /// AND - Logical AND
    /// A logical AND is performed, bit by bit, on the accumulator contents
    /// using the contents of a byte of memory.
    void logical_and(uint8_t val);
    /// EOR - Exclusive OR
    /// An exclusive OR is performed, bit by bit, on the accumulator contents
    /// using the contents of a byte of memory.
    void eor(uint8_t val);
    /// ORA - Logical Inclusive OR
    /// An inclusive OR is performed, bit by bit, on the accumulator contents
    /// using the contents of a byte of memory.
    void ora(uint8_t val);
    /// BIT - Bit Test
    /// This instructions is used to test if one or more bits are set in a
    /// target memory location. The mask pattern in A is ANDed with the value
    /// in memory to set or clear the zero flag, but the result is not kept.
    /// Bits 7 and 6 of the value from memory are copied into the N and V
    /// flags.
    void bit(uint8_t val);

    /**************
     * Arithmetic *
     **************/
    /// ADC - Add with CARRY
    /// This instruction adds the contents of a memory location to the
    /// accumulator together with the carry bit. If overflow occurs the carry
    /// bit is set, this enables multiple byte addition to be performed.
    void adc(uint8_t val);
    /// SBC - Subtract with CARRY
    /// This instruction subtracts the contents of a memory location to the
    /// accumulator together with the not of the carry bit. If overflow occurs
    /// the carry bit is clear, this enables multiple byte subtraction to be
    /// performed.
    void sbc(uint8_t val);
    /// Compares the contents of the given register with another
    /// memory held value and sets the zero and carry flags as appropriate.
    void compare(uint8_t reg, uint8_t val);
    /// CMP - Compare
    /// This instruction compares the contents of the accumulator with another
    /// memory held value and sets the zero and carry flags as appropriate.
    void cmp(uint8_t val);
    /// CPX - Compare X Register
    /// This instruction compares the contents of the X register with another
    /// memory held value and sets the zero and carry flags as appropriate.
    void cpx(uint8_t val);
    /// CPY - Compare Y Register
    /// This instruction compares the contents of the Y register with another
    /// memory held value and sets the zero and carry flags as appropriate.
    void cpy(uint8_t val);

    /***************************
     * Increments & Decrements *
     ***************************/
    /// INC - Increment a memory location
    /// Adds one to the value held at a specified memory location setting the
    /// zero and negative flags as appropriate.
    void inc(uint16_t addr);
    /// INX - Increment the X register
    /// Adds one to the X register setting the zero and negative flags as
    /// appropriate.
    void inx();
    /// INY - Increment the Y register
    /// Adds one to the Y register setting the zero and negative flags as
    /// appropriate.
    void iny();
    /// DEC - Decrement a memory location
    /// Subtracts one from the value held at a specified memory location
    /// setting the zero and negative flags as appropriate.
    void dec(uint16_t addr);
    /// DEX - Decrement the X register
    /// Subtracts one from the X register setting the zero and negative flags
    /// as appropriate.
    void dex();
    /// DEY - Decrement the Y register
    /// Subtracts one from the Y register setting the zero and negative flags
    /// as appropriate.
    void dey();

    /**********
     * Shifts *
     **********/
    /// ASL - Arithmetic Shift Left
    /// This operation shifts all the bits of the accumulator or memory
    /// contents one bit left. Bit 0 is set to 0 and bit 7 is placed in the
    /// carry flag. The effect of this operation is to multiply the memory
    /// contents by 2 (ignoring 2's complement considerations), setting the
    /// carry if the result will not fit in 8 bits.
    void asl(uint8_t *val);
    /// LSR - Logical Shift Right
    /// Each of the bits in A or M is shift one place to the right. The bit
    /// that was in bit 0 is shifted into the carry flag. Bit 7 is set to zero.
    void lsr(uint8_t *val);
    /// ROL - Rotate Left
    /// Move each of the bits in either A or M one place to the left. Bit 0 is
    /// filled with the current value of the carry flag whilst the old bit 7
    /// becomes the new carry flag value.
    void rol(uint8_t *val);
    /// ROR - Rotate Right
    /// Move each of the bits in either A or M one place to the right. Bit 7 is
    /// filled with the current value of the carry flag whilst the old bit 0
    /// becomes the new carry flag value.
    void ror(uint8_t *val);

    /*****************
     * Jumps & Calls *
     *****************/
    /// JMP - Jump to another location
    /// Sets the program counter to the address specified by the operand.
    void jmp(uint16_t addr);
    /// JSR - Jump to a subroutine
    /// The JSR instruction pushes the address (minus one) of the return point
    /// on to the stack and then sets the program counter to the target memory
    /// address.
    void jsr(uint16_t addr);
    /// RTS - Return from subroutine
    /// The RTS instruction is used at the end of a subroutine to return to the
    /// calling routine. It pulls the program counter (minus one) from the
    /// stack.
    void rts();

    /************
     * Branches *
     ************/
    /// BCC - Branch if carry flag clear
    /// If the carry flag is clear then add the relative displacement to the
    /// program counter to cause a branch to a new location.
    void bcc();
    /// BCS - Branch if carry flag set
    /// If the carry flag is set then add the offset displacement to the
    /// program counter to cause a branch to a new location.
    void bcs();
    /// BEQ - Branch if zero flag set
    /// If the zero flag is set then add the offset displacement to the
    /// program counter to cause a branch to a new location.
    void beq();
    /// BMI - Branch if negative flag set
    /// If the negative flag is set then add the offset displacement to the
    /// program counter to cause a branch to a new location.
    void bmi();
    /// BNE - Branch if zero flag clear
    /// If the zero flag is clear then add the offset displacement to the
    /// program counter to cause a branch to a new location.
    void bne();
    /// BPL - Branch if negative flag clear
    /// If the negative flag is clear then add the offset displacement to the
    /// program counter to cause a branch to a new location.
    void bpl();
    /// BVC - Branch if overflow flag clear
    /// If the overflow flag is clear then add the offset displacement to the
    /// program counter to cause a branch to a new location.
    void bvc();
    /// BVS - Branch if overflow flag set
    /// If the overflow flag is set then add the offset displacement to the
    /// program counter to cause a branch to a new location.
    void bvs();

    /***********************
     * Status Flag Changes *
     ***********************/
    /// CLC - Clear carry flag
    /// Set the carry flag to zero.
    void clc();
    /// CLD - Clear decimal mode flag
    /// Sets the decimal mode flag to zero.
    void cld();
    /// CLI - Clear interrupt disable flag
    /// Clears the interrupt disable flag allowing normal interrupt requests to
    /// be serviced.
    void cli();
    /// CLV - Clear overflow flag
    /// Clears the overflow flag.
    void clv();
    /// SEC - Set carry flag
    /// Set the carry flag to one.
    void sec();
    /// SED - Set decimal mode flag
    /// Set the decimal mode flag to one.
    void sed();
    /// SEI - Set interrupt disable flag
    /// Set the interrupt disable flag to one.
    void sei();

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
};

}   // Namespace cpu.