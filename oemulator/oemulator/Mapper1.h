#pragma once
#include "Mapper.h"

class Mapper1 : public Mapper {
public:
	Mapper1(iNESHeader header, byte rom[]);

	byte read(int addr);
	void write(int addr, byte val);

private:
	//standard 8kb PRGRAM at 0x6000-0x7fff;
	byte PRGRAM[s8KB];
	//Mapped to CPU 0x8000-0xbfff and 0xc000-0xffff (16kb units)
	byte PRGROM[0x40000];
	int currentPRGBank;
	int numPRGBanks;
	//Mapped to PPU 0x000-0x0fff and 0x1000-0x1fff (4kb units)
	byte CHRROM[0x20000];
	int currentCHRBank;
	int numCHRBanks;

	int shiftRegister;
};