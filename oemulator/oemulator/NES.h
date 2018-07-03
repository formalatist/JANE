#pragma once
#include "CPU6502.h"
#include "PPU.h"
#include "Memory.h"
#include "PPUMemory.h"
#include <SDL.h>

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

	void setScreen(SDL_Surface* screen_, SDL_Window* window_);
	void updateScreen();

	//the CPU and the PPU, they talk to eachother via the NES
	//so they need to be public
	CPU6502* cpu;
	PPU* ppu;
	Memory* memory;
	PPUMemory* ppuMemory;

private:
	SDL_Surface* screen;
	SDL_Window* window;
};