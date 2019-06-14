#include "ROMLoader.h"
#include <iostream>

ROMLoader::ROMLoader(Memory* memory_, PPUMemory* ppuMemory_)
{
	memory = memory_;
	ppuMemory = ppuMemory_;
}

void ROMLoader::loadROM(byte rom[], int size, CPU6502& cpu)
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
	//TODO: properly read the ines header. this is a placeholder that only works for donkey kong
	//this does NOT support RAM on the chip. only rom. add that later ;^)
	int PRGROMSIZE = rom[4] * 16384;
	int CHRROMSIZE = rom[5] * 8192;
	ppuMemory->setMirror(rom[6]&1);
	int mapperNumber = ((rom[6] & 0xf0)>>4) | (rom[7] & 0xf0);
	std::cout << "Mapper number: " << std::dec << mapperNumber << std::endl;

	byte * rom2 = new byte[size - headerSize];
	for (int i = 0; i < size - headerSize; i++) {
		rom2[i] = rom[i + headerSize];
	}
	Mapper0 mapper = Mapper0(rom, rom2);
	memory->setMapper(&mapper);

	byte * chrROM = new byte[CHRROMSIZE];
	//read in the chrROM
	for (int i = 0; i < CHRROMSIZE; i++) {
		chrROM[i] = rom[i + headerSize + PRGROMSIZE];
	}
	//load the rom into the ppu
	ppuMemory->loadMemory(chrROM, CHRROMSIZE, 0x0000);

	//set the PC to the init vector
	std::cout << std::hex << (int)memory->memory[0xfffc] << "  " << (memory->memory[0xfffd] << 8) << std::endl;
	cpu.PC = memory->read(0xfffc) | ((memory->read(0xfffd) << 8));
	//FOR NESTEST ONLY
	//cpu.PC = 0xC000;
}

void ROMLoader::clearPPUReg()
{
	for (int i = 0x2000; i < 0x2008; i++)
	{
		memory->write(i, 0);
	}
}
