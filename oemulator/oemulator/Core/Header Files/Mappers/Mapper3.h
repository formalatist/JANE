#pragma once
#include "Mapper.h"

class Mapper3 : public Mapper {
public:
	Mapper3(iNESHeader header, byte rom[]);

	byte read(int addr);
	void write(int addr, byte val);

private:
	// 16kb/32kb PRGROM
	byte PRGROM[s32KB];

	//CHRROM is upto 32KB, 8KB banks at 0x0000-0x1fff
	byte CHRROM[s32KB];
	int currentCHRBank;
};