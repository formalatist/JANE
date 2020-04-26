#pragma once
#include "Mapper.h"


class Mapper1 : public Mapper {
public:
	Mapper1(iNESHeader header, byte rom[]);

	byte read(int addr);
	void write(int addr, byte val);

private:
	//TODO more than 8kb is only used in SOROM, SUROM and SXROM (i think)
	//32kb PRGRAM at 8kb window 0x6000-0x7fff;
	byte PRGRAM[s8KB] = { 0 };
	//Mapped to CPU 0x8000-0xbfff and 0xc000-0xffff (16kb units)
	byte PRGROM[0x40000];
	int PRGBank1; //0x8000-0xbfff
	int PRGBank2; //0xc000-0xffff
	int numPRGBanks;
	/*PRG ROM swap bank
		0 - Bank 8000-BFFFh is fixed, while C000-FFFFh is swappable
		1 - Bank C000-FFFFh is fixed, while 8000-FFFFh is swappable. (power-on default)*/
	int PRGSwapMode;
	/*PRG Bank size.
		0 - Swappable bank is 32K in size.
		1 - Swappable bank is 16K in size.*/
	int PRGBankSize;
	int controlReg;
	int CHRROMReg1;
	int CHRROMReg2;
	int PRGROMReg;

	//Mapped to PPU 0x000-0x0fff and 0x1000-0x1fff (4kb units)
	byte CHRROM[0x20000] = { 0 };
	int CHRBank1; //0x0000-0x0fff
	int CHRBank2; //0x1000-0x1fff
	int numCHRBanks;
	/*CHR Bank size.
		0 - Single 8K bank in CHR space.
		1 - Two 4K banks in CHR space.*/
	int CHRBankSize;
	int mirror;

	int shiftRegister; //internal register that holds 5 bits
	int shiftRegisterWriteCounter; 

	void writeRegister(int addr);

	void updateBanksAndMirror();
};