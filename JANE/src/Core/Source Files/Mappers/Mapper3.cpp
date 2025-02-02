#include "Mappers/Mapper3.h"

Mapper3::Mapper3(iNESHeader header, byte rom[])
{
	//setup PRGROM bank
	for (int i = 0; i < header.numPRGROMUnits*s16KB; i++) {
		PRGROM[i] = rom[i];
	}

	//setup CHRROM
	for (int i = 0; i < header.numCHRROMUnits*s8KB; i++) {
		CHRROM[i] = rom[i + header.numPRGROMUnits*s16KB];
	}
	currentCHRBank = 0;

	//set mirroring
	if (header.verticalMirroring) {
		mirrorMode = Vertical;
	}
	else {
		mirrorMode = Horizontal;
	}
}

byte Mapper3::read(int addr)
{
	if (addr <= 0x1fff) {
		return CHRROM[addr + currentCHRBank*s8KB];
	}
	else if(addr >= 0x8000 && addr <= 0xffff) { //PRGROM
		return PRGROM[addr-0x8000];
	}
	return 0;
}

void Mapper3::write(int addr, byte val)
{
	if (addr <= 0x1fff) {
		std::cout << "WRITING TO CHR RAM" << std::endl;
	}
	if(addr >= 0x8000 && addr <= 0xffff) { //bank select using 2 lowest bits
		//TODO: some boards use all bits and support upto 2mb CHRROM
		currentCHRBank = val & 3;
		std::cout << "SWITCHING CHR BANK: " << (int)(val&3) << std::endl;
	}
}
