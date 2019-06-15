#pragma once
#include <iostream>
#include "Mapper.h"

typedef unsigned char byte;

class Mapper0 : public Mapper {
public:
	//constructor
	Mapper0(byte header[], byte rom[]);

	void write(int addr, byte val);
	byte read(int addr);

private:
	//8 kb window of PRGRAM located at CPU 0x6000-0x7FFF
	//Mapper0 has either 2 or 4kb, this is mirrored
	//to fill the entire 8kb (Mirrored in Family Basic only)
	byte PRGRAM[s8KB];

	//First 16kb of PRG ROM, located at CPU 0x8000-0xBFFF
	byte PRGROM1[s16KB];
	//Last 16kb of PRG ROM (if mapper is NROM-256) or mirror of
	// 0x8000-0xBFFF (if mapper is NROM-128), located at 0xC000-0xFFFF
	byte PRGROM2[s16KB];

	//CHRROM at 0x0000-0x1fff, 8KB
	byte CHRROM[s8KB];
};