# CMakeList.txt : CMake project for nes-emu, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
add_executable (
	nes-emu 
	"main.cpp" "main.h" 
	"cpu/cpu.cpp" "cpu/cpu.h" "cpu/addressing.cpp" "cpu/instructions.cpp"
	"ram/ram.cpp" "ram/ram.h"
)

# TODO: Add tests and install targets if needed.
