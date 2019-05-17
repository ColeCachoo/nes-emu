// main.cpp : Defines the entry point for the application.
//

#include "main.h"

using namespace std;

// void tests();

int main() {
	cout << "Hello nes-emu." << endl;

	RAM ram;
	for (uint8_t i = 0; i < 200; i++) {
		ram.write(i, i);
	}
	CPU cpu(&ram);
	for (uint8_t i = 0; i < 200; i++) {
		cout << int(ram.read(i)) << " ";
	}
	puts("");

	//tests();
	return 0;
}

/*
void tests() {
	CPU cpu;

	cout << "\n\nStarting tests...\n\n";
	cout << cpu.get_cpu_status() << endl;

	cpu.sec();
	if (cpu.get_cpu_status() == 0x01) {
		cout << "SEC ................ Passed\n";
	} else {
		cout << "SEC ................ Failed\n";
	}
	cpu.clc();
	if (cpu.get_cpu_status() == 0) {
		cout << "CLC ................ Passed\n";
	} else {
		cout << "CLC ................ Failed\n";
	}

	cpu.sed();
	if (cpu.get_cpu_status() == 0x08) {
		cout << "SED ................ Passed\n";
	} else {
		cout << "SED ................ Failed\n";
	}
	cpu.cld();
	if (cpu.get_cpu_status() == 0) {
		cout << "CLD ................ Passed\n";
	} else {
		cout << "CLD ................ Failed\n";
	}

	cpu.sei();
	if (cpu.get_cpu_status() == 0x04) {
		cout << "SEI ................ Passed\n";
	} else {
		cout << "SEI ................ Failed\n";
	}
	cpu.cli();
	if (cpu.get_cpu_status() == 0) {
		cout << "CLI ................ Passed\n";
	} else {
		cout << "CLI ................ Failed\n";
	}
}
*/