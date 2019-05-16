// ram.cpp : Initializes the array that will act as RAM.
//
#include "ram.h"

using namespace std;

RAM::RAM() {
	// 64Kb RAM.
	memory = vector<uint8_t>(64 * 1024);
}
