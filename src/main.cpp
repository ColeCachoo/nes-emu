// main.cpp : Defines the entry point for the application.
//
#include "cpu/cpu.h"
#include "ppu/ppu.h"
#include "map.h"
#include "nes-error.h"
#include "nes-utils.h"
// TODO: Remove when done.
// #include "sdl2-playground.h"

#include <SDL.h>
#include <fmt/format.h>
#include <fmt/ostream.h>

#include <fstream>
#include <memory>
#include <string>
#include <vector>

using namespace std;

// void sdl_playground();
// SDL_Surface *load_surface(string path);

void parse_results();
void compare_logs();

int main(int argc, char *args[]) {
    // sdl_playground();
    fmt::print("Hello nes-emu.\n");

    // 64kB of RAM.
    auto ram = make_unique<uint8_t[]>(64 * 1024);
    // 16kB of VRAM
    auto vram = make_unique<uint8_t[]>(16 * 1024);
    // uint8_t vram[16 * 1024];

    if (ram == nullptr || vram == nullptr) {
        fmt::print(stderr, "Failed to allocate for RAM or VRAM.\n");
        exit(ERR_NO_ALLOC);
    }

    // if (map("../resources/nestest.nes", ram.get()) == ERROR) {
    if (map("../resources/nestest.nes", ram.get(), vram.get()) == ERROR) {
        fmt::print(stderr, "Failed to open ROM.\n");
        exit(1);
    }

    cpu::CPU cpu(ram.get());
    cpu.run();

    ppu::PPU ppu(ram.get(), vram.get());

    // parse_results();
    compare_logs();

    return 0;
}

void parse_results() {
    const string test_log = "nestest.log.txt";
    ifstream cpu_test_log("../resources/" + test_log);
    if (!cpu_test_log.is_open()) {
        fmt::print(stderr, "Failed to open {}\n", test_log);
        return;
    }

    ofstream parsed_log("../resources/parsed_log.txt");
    if (!parsed_log.is_open()) {
        fmt::print("Failed to open file for writing.\n");
        exit(1);
    }


    string line;
    while(getline(cpu_test_log, line)) {
        line += "\n";
        vector<string> tokens;
        size_t tok_beg = line.find_first_not_of(" \t\n");
        size_t tok_end = line.find_first_of(" \t\n");
        while (tok_beg != string::npos || tok_end != string::npos) {
            string tok = "";
            while (tok_beg != tok_end) {
                tok += line.at(tok_beg);
                tok_beg++;
            }
            tokens.push_back(tok);

        tok_beg = line.find_first_not_of(" \t\n", tok_beg);
        tok_end = tok_beg;
        tok_end = line.find_first_of(" \t\n", tok_end);
        }

        string result = "";
        result += tokens[0] + " " + tokens[1] + " ";
        for (size_t i = 2; i < tokens.size(); i++) {
            if (tokens.at(i).find("A:") != string::npos) {
                result += tokens.at(i) + " ";
                result += tokens.at(i + 1) + " ";
                result += tokens.at(i + 2) + " ";
                result += tokens.at(i + 3) + " ";
                result += tokens.at(i + 4);
                break;
            }
        }
        fmt::print(parsed_log, "{}\n", result);
    }

    cpu_test_log.close();
    parsed_log.close();
}

void compare_logs() {
    ifstream my_log("../resources/my_log.txt");
    ifstream parsed_log("../resources/parsed_log.txt");
    if (!my_log.is_open() || !parsed_log.is_open()) {
        fmt::print(stderr, "Failed to open log for comparison\n");
        exit(1);
    }

    string my_log_line = "";
    string parsed_log_line = "";
    string old_my_line = "";
    string old_parsed_line = "";
    while (getline(my_log, my_log_line) && getline(parsed_log, parsed_log_line)) {
        if (my_log_line != parsed_log_line) {
            fmt::print("Expected\n{}\n{}\n", old_parsed_line, parsed_log_line);
            fmt::print("Got\n{}\n{}\n", old_my_line, my_log_line);
            break;
        }
        old_my_line = my_log_line;
        old_parsed_line = parsed_log_line;
    }
}