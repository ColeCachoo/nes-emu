# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("nes-emu"
		DESCRIPTION "NES emulator"
		LANGUAGES    CXX
)

# SET(GCC_COVERAGE_COMPILE_FLAGS "-Wall -Wextra -O2")
# add_definitions(${GCC_COVERAGE_COMPILE_FLAGS})

# Include sub-projects.
add_subdirectory ("src")

# add_executable (
# 	nes-emu 
# 	"src/main.cpp" "src/main.h" 
# 	"src/cpu/cpu.cpp" "src/cpu/cpu.h" "src/cpu/addressing.cpp" "src/cpu/instructions.cpp"
# 	"src/ram/ram.cpp" "src/ram/ram.h"
# )

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	target_compile_options(nes-emu PRIVATE -Wall -Wextra -O2)
endif()
