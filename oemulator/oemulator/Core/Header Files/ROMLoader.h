#pragma once
#define _CRT_SECURE_NO_DEPRECATE
//#include "Memory.h"
//#include "CPU6502.h"
#include "NES.h"
#include "ROMInfo.h"

class Memory;
class PPUMemory;
class CPU6502;

typedef unsigned char byte;
class ROMLoader {
public:
	//constructor
	ROMLoader(Memory* memory_, PPUMemory* ppuMemory_);

	void loadROM(byte rom[], int size, CPU6502& cpu, byte prgram[] = nullptr);

	void loadROMFromFile(std::string filepath, CPU6502& cpu);

	void loadROMFromROMInfo(ROMInfo ROMInfo_, CPU6502& cpu);

	iNESHeader getHeader(byte rom[]);

	//in future the PPU might be setting these itself
	void clearPPUReg();

	//FOR DEBUG, MOVE TO SENSIBLE PLACE
	Mapper* mapper;

private:
	long getFileSize(FILE *file);

	Memory* memory;
	PPUMemory* ppuMemory;
};