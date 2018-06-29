#include "NES.h"

NES::NES()
{
	memory = new Memory();
	ppuMemory = new PPUMemory();
	cpu = new CPU6502(memory);
	ppu = new PPU();
	
	//tell the memory about the cpu and ppu
	memory->setCPU(*cpu);
	memory->setPPU(*ppu);
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
