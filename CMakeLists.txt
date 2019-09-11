cmake_minimum_required (VERSION 3.8)

if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
      CACHE STRING "")
endif()

project(nes-emu CXX)

find_package(fmt REQUIRED)
find_package(SDL2 REQUIRED)
# find_package(SDL2-image REQUIRED)

add_executable (
    ${PROJECT_NAME} 
    "src/main.cc"
    "src/nes-error.h"
    "src/nes-utils.h"
    "src/cpu/cpu.h" 
    "src/cpu/cpu.cc" 
    "src/cpu/instructions.cc"
    "src/map.h"
    "src/map.cc" 
    "src/ppu/ppu.h"
    "src/ppu/ppu.cc"
)
    # "src/sdl2-playground.cpp"
    # "src/sdl2-playground.h"
    # "src/ram.h"

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_LIST_DIR}/src")

target_link_libraries(${PROJECT_NAME} fmt::fmt)
target_link_libraries(${PROJECT_NAME} SDL2::SDL2 SDL2::SDL2main)
# target_link_libraries(${PROJECT_NAME} SDL2::SDL2_image)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

if(CMAKE_COMPILER_IS_GNUCXX OR LLVM)
    # target_compile_options(nes-emu PRIVATE -pedantic -Wall -Wextra -O2)
    target_compile_options(nes-emu PRIVATE -pedantic -pedantic-errors -Wall -Wextra)
elseif(MSVC)
    # TODO: Remove this compile option once fmt is updated.
    # wd4275 disables a warning triggered by fmt.
    target_compile_options(nes-emu PRIVATE /W4 /wd4275)
    # target_compile_options(nes-emu PRIVATE /W4)
endif()

# enable_testing()
# find_package(GTest MODULE REQUIRED)
# target_link_libraries(main PRIVATE GTest::GTest GTest::Main)
# add_test(AllTestsInMain main)