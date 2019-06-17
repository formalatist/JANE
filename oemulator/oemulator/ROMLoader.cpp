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
	int mapperNumber = ((rom[6] & 0xf0) >> 4) | (rom[7] & 0xf0);
	std::cout << "Mapper number: " << std::dec << mapperNumber << std::endl;

	byte * rom2 = new byte[size-headerSize];
	for (int i = 0; i < size-headerSize; i++) {
		rom2[i] = rom[i + headerSize];
	}

	iNESHeader header = getHeader(rom);
	if (mapperNumber == 0) {
		mapper = new Mapper0(rom, rom2);
	}
	else if (mapperNumber == 2) {
		mapper = new Mapper2(header, rom2);
	}
	else if (mapperNumber == 3) {
		mapper = new Mapper3(header, rom2);
	}
	memory->setMapper(mapper);
	ppuMemory->setMapper(mapper);
	ppuMemory->setMirror(header.verticalMirroring);

	//set the PC to the init vector
	std::cout << std::hex << (int)memory->memory[0xfffc] << "  " << (memory->memory[0xfffd] << 8) << std::endl;
	cpu.PC = memory->read(0xfffc) | ((memory->read(0xfffd) << 8));
}

iNESHeader ROMLoader::getHeader(byte rom[])
{
	iNESHeader header = iNESHeader(rom[4], rom[5], rom[6]&1, 0, 0, 0);
	return header;
}

void ROMLoader::clearPPUReg()
{
	for (int i = 0x2000; i < 0x2008; i++)
	{
		memory->write(i, 0);
	}
}
