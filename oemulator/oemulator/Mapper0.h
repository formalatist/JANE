#pragma once
#include <iostream>

typedef unsigned char byte;

class Mapper0 {
public:
	//constructor
	Mapper0(byte header[], byte rom[]);

	void write(int addr, byte val);
	byte read(int addr);

private:
	//8 kb window of PRGRAM located at CPU 0x6000-0x7FFF
	//Mapper0 has either 2 or 4kb, this is mirrored
	//to fill the entire 8kb (Mirrored in Family Basic only)
	byte PRGRAM[0x2000];

	//First 16kb of PRG ROM, located at CPU 0x8000-0xBFFF
	byte PRGROM1[0x4000];
	//Last 16kb of PRG ROM (if mapper is NROM-256) or mirror of
	// 0x8000-0xBFFF (if mapper is NROM-128), located at 0xC000-0xFFFF
	byte PRGROM2[0x4000];
};