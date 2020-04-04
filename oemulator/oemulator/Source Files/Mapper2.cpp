#include "Mapper2.h"

Mapper2::Mapper2(iNESHeader header, byte rom[])
{
	//setup PRG
	PRGBank1 = 0;
	PRGBank2 = (header.numPRGROMUnits-1); //fixed to the last bank
	for (int i = 0; i < s16KB*header.numPRGROMUnits; i++) {
		PRGROM[i] = rom[i];
	}

	//setup CHR
	for (int i = 0; i < s8KB*header.numCHRROMUnits; i++) {
		CHRROM[i] = rom[i + s16KB*header.numPRGROMUnits];
	}

	//set mirroring
	if (header.verticalMirroring) {
		mirrorMode = Vertical;
	}
	else {
		mirrorMode = Horizontal;
	}
}

byte Mapper2::read(int addr)
{
	if (addr <= 0x1fff) {
		return CHRROM[addr];
	}
	else if (addr >= 0x8000 && addr <= 0xbfff) {
		return PRGROM[addr + PRGBank1*s16KB - 0x8000];
	}
	else {
		return PRGROM[addr + PRGBank2*s16KB - 0xc000];
	}
}

void Mapper2::write(int addr, byte val)
{
	if (addr <= 0x1fff) {
		CHRROM[addr] = val;
	}
	else if (addr >= 0x8000 && addr <= 0xffff) {
		PRGBank1 = val & 0x7;
	}
}
