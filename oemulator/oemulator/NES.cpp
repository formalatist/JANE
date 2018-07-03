#include "NES.h"

NES::NES()
{
	memory = new Memory();
	ppuMemory = new PPUMemory();
	cpu = new CPU6502(memory);
	ppu = new PPU(ppuMemory);
	
	//tell the memory about the cpu and ppu
	memory->setCPU(*cpu);
	memory->setPPU(*ppu);

	//tell the PPU about the NES
	ppu->setNES(this);
}

void NES::step(int numSteps)
{
	for (int i = 0; i < numSteps; i++) {
		int cpuCyclesDone = cpu->step();
		int ppuCyclesNeeded = cpuCyclesDone * 3;
		for (int j = 0; j < ppuCyclesNeeded; j++) {
			ppu->step();
		}
	}
}

void NES::stepSeconds(float seconds)
{
	int cyclesNeeded = (int)(cpu->CPUFrequency * seconds);
	step(cyclesNeeded);
}

void NES::setScreen(SDL_Surface * screen_, SDL_Window* window_)
{
	screen = screen_;
	window = window_;
}

void NES::updateScreen()
{
	for (int x = 0; x < 512; x++) {
		for (int y = 0; y < 240; y++) {

			Uint8 *targetPixel = (Uint8*)screen->pixels + (y )*screen->pitch
				+ (x) * 4;
			*(Uint32 *)targetPixel = ppu->pixels[x + y * 512];
		}
	}
	SDL_UpdateWindowSurface(window);
}
