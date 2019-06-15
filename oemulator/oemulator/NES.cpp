#include "NES.h"
#include <ctime>

NES::NES()
{
	memory = new Memory();
	ppuMemory = new PPUMemory();
	cpu = new CPU6502(memory);
	ppu = new PPU(ppuMemory);
	
	//tell the memory about the cpu and ppu
	memory->setCPU(*cpu);
	memory->setPPU(*ppu);

	//tell the PPUMemory about cpu
	ppuMemory->setCPU(*cpu);

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
	int cyclesDone = 0;
	while (true) {
		int cpuCyclesDone = cpu->step();
		int ppuCyclesNeeded = cpuCyclesDone * 3;
		for (int j = 0; j < ppuCyclesNeeded; j++) {
			ppu->step();
		}
		cyclesDone += cpuCyclesDone;
		if (cyclesDone >= cyclesNeeded) {
			return;
		}
	}
}

void NES::setScreen(SDL_Surface * screen_, SDL_Surface* scaledScreen_, SDL_Window* window_)
{
	screen = screen_;
	scaledScreen = scaledScreen_;
	window = window_;
}

//TODO: move this to a "display" object or something
void NES::updateScreen()
{
	//int start = clock();
	int s = 2;
	for (int x = 0; x < 512; x++) {
		for (int y = 0; y < 240; y++) {
			//original scale = 1 rendering
			/*Uint8 *targetPixel = (Uint8*)screen->pixels + (y)*screen->pitch
				+ (x) * 4;
			*(Uint32 *)targetPixel = ppu->pixels[x + y * 512];*/

			Uint8 *targetPixel = (Uint8*)screen->pixels + (y*s)*screen->pitch
				+ (x*s) * 4;
			*(Uint32 *)targetPixel = ppu->pixels[x + y * 512];
			targetPixel = (Uint8*)screen->pixels + (y*s + 1)*screen->pitch
				+ (x*s) * 4;
			*(Uint32 *)targetPixel = ppu->pixels[x + y * 512];
			targetPixel = (Uint8*)screen->pixels + (y*s)*screen->pitch
				+ (x*s+1) * 4;
			*(Uint32 *)targetPixel = ppu->pixels[x + y * 512];
			targetPixel = (Uint8*)screen->pixels + (y*s + 1)*screen->pitch
				+ (x*s+1) * 4;
			*(Uint32 *)targetPixel = ppu->pixels[x + y * 512];
		}
	}
	SDL_Rect rect;
	rect.x = 0;
	rect.y = 0;
	rect.w = 512 * 2;
	rect.h = 240 * 2;
	//SDL_BlitScaled(screen, 0, scaledScreen, &rect);
	SDL_UpdateWindowSurface(window);
	//std::cout << "Frame time: " << (clock() - start) / double(CLOCKS_PER_SEC) * 1000 << std::endl;
}

void NES::connectController(Controller * controller)
{
	controller->setMemory(memory);
	memory->connectController(controller);
}
