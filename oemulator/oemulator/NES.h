#pragma once
#include "CPU6502.h"
#include "PPU.h"
#include "Memory.h"
#include "PPUMemory.h"


class NES {
public:
	//constructor
	NES();

	//do a number of cpu steps. (numSteps*3 PPU steps will be done)
	void step(int numSteps);
	//step an amount of seconds forward in time
	void stepSeconds(float seconds);
	//do one whole frame
	void stepFrame();

	//the CPU and the PPU, they talk to eachother via the NES
	//so they need to be public
	CPU6502* cpu;
	PPU* ppu;
	Memory* memory;
	PPUMemory* ppuMemory;

private:
	
};