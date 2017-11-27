#pragma once
#include "CPU6502.h"

class ROMLoader {
public:
	//constructor
	ROMLoader(CPU6502* cpu_);

	void loadROM(byte rom[], int size);

	//in future the PPU might be setting these itself
	void clearPPUReg();

private:
	CPU6502* cpu;
};