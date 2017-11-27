#include "ROMLoader.h"
#include <iostream>

ROMLoader::ROMLoader(CPU6502 * cpu_)
{
	cpu = cpu_;
}

void ROMLoader::loadROM(byte rom[], int size)
{
	const int headerSize = 16;
	//iNES format
	/*
	1. Header(16 bytes)
	2. Trainer, if present(0 or 512 bytes)
	3. PRG ROM data(16384 * x bytes)
	4. CHR ROM data, if present(8192 * y bytes)
	5. PlayChoice INST - ROM, if present(0 or 8192 bytes)
	6. PlayChoice PROM, if present(16 bytes Data, 16 bytes CounterOut) (this is often missing, see PC10 ROM - Images for details)
	*/
	//TODO: properly red the ines header. this is a placeholder that only works for donkey kong
	byte prgROM[16384];
	//read inn the prgROM
	for (int i = 0; i < 16384; i++)
	{

		prgROM[i] = rom[i + headerSize];
	}

	//load the rom into the cpu
	cpu->loadMemory(prgROM, 16384, 0x8000);
	cpu->loadMemory(prgROM, 16384, 0xc000);

	//set the PC to the init vector
	std::cout << std::hex << (int)cpu->memory[0xfffc] << "  " << (cpu->memory[0xfffd] << 8) << std::endl;
	cpu->PC = cpu->memory[0xfffc] | (cpu->memory[0xfffd] << 8);
}

void ROMLoader::clearPPUReg()
{
	for (int i = 0x2000; i < 0x2008; i++)
	{
		cpu->memory[i] = 0;
	}
}
