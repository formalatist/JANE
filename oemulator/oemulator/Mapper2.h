#pragma once
#include "Mapper.h"

class Mapper2 : public Mapper {
public:
	Mapper2(iNESHeader header, byte rom[]);

	byte read(int addr);
	void write(int addr, byte val);

private:
	//CPU 0x8000-0xBFFF: 16 KB switchable PRG ROM bank
	//CPU 0xC000 - 0xFFFF : 16 KB PRG ROM bank, fixed to the last bank
	byte PRGROM[s256KB];
	int PRGBank1; //switchable bank
	int PRGBank2; //fixed bank

	//this mapper has CHR ram at 0x0000-0x1fff
	byte CHRROM[s8KB];
};