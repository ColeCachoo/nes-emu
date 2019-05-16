// ram.h : RAM class just gives an interface to access the array used as RAM.
//
#pragma once

#include <cstdint>
#include <vector>

class RAM {
public:
	RAM();
	~RAM() = default;

	/// Reads address from memory and returns its contents.
	uint8_t read(uint16_t addr) const {
		return memory.at(addr);
	}

	/// Writes data to address.
	void write(uint8_t val, uint16_t addr) {
		memory.at(addr) = val;
	}

private:
	/// The RAM.
	std::vector<uint8_t> memory;
};
