#include "NES.h"

void NES::step(int numSteps)
{
	for (int i = 0; i < numSteps; i++) {
		int cpuCyclesDone = cpu.step();
		int ppuCyclesNeeded = cpuCyclesDone * 3;
		for (int j = 0; j < ppuCyclesNeeded; j++) {
			ppu.step();
		}
	}
}

void NES::stepSeconds(float seconds)
{
	int cyclesNeeded = (int)(cpu.CPUFrequency * seconds);
	step(cyclesNeeded);
}
