// addressing.cpp : Defines the addressing mode instructions used by the CPU
// instructions.
//
#include "cpu.h"

/*
uint8_t 
CPU::implicit() {

}

uint8_t 
CPU::accum() {

}
*/

uint8_t 
CPU::immediate() {
	return fetch();
}

uint8_t 
CPU::zero_page() {
	return ram->read(fetch());
}

uint8_t 
CPU::zero_page_x() {
	return ram->read(x_index + fetch() % 256);
}

uint8_t 
CPU::zero_page_y() {
	return ram->read(y_index + fetch() % 256);
}

int8_t 
CPU::relative() {
	return int8_t(fetch());
}

uint8_t 
CPU::absolute() {
	uint16_t addr_low  = fetch();
	uint16_t addr_high = fetch();
	addr_high <<= 8;
	return ram->read(addr_high | addr_low);
}

uint8_t 
CPU::absolute_x() {
	uint16_t addr_low  = fetch();
	uint16_t addr_high = fetch();
	addr_high <<= 8;
	return ram->read((addr_high | addr_low) + x_index);
}

uint8_t 
CPU::absolute_y() {
	uint16_t addr_low  = fetch();
	uint16_t addr_high = fetch();
	addr_high <<= 8;
	return ram->read((addr_high | addr_low) + y_index);
}

/* Special to JMP, so just implement in JMP? */
uint16_t 
CPU::indirect() {
	uint16_t addr_low  = fetch();
	uint16_t addr_high = fetch();
	addr_high <<= 8;
	uint16_t new_low  = ram->read(addr_high | addr_low);
	uint16_t new_high = ram->read((addr_high | addr_low) + 1);
	new_high <<= 8;
	return (new_high | new_low);
}

uint8_t 
CPU::indexed_indirect() {
	uint8_t arg = fetch();
	return ram->read(ram->read((arg + x_index) % 256) + ram->read((arg + x_index + 1) % 256) * 256);

}

uint8_t 
CPU::indirect_indexed() {
	uint8_t arg = fetch();
	return ram->read(ram->read(arg) + ram->read((arg + 1) % 256) * 256 + y_index);
}
