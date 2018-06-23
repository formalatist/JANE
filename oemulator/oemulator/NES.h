#pragma once
#include "CPU6502.h"
#include "PPU.h"

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

private:
	CPU6502 cpu;
	PPU ppu;
};