#pragma once
//#include "Memory.h"
//#include "CPU6502.h"
#include "NES.h"
class Memory;
class PPUMemory;
class CPU6502;

typedef unsigned char byte;
class ROMLoader {
public:
	//constructor
	ROMLoader(Memory* memory_, PPUMemory* ppuMemory_);

	void loadROM(byte rom[], int size, CPU6502& cpu);

	iNESHeader getHeader(byte rom[]);

	//in future the PPU might be setting these itself
	void clearPPUReg();

	//FOR DEBUG, MOVE TO SENSIBLE PLACE
	Mapper* mapper;

private:
	Memory* memory;
	PPUMemory* ppuMemory;
};