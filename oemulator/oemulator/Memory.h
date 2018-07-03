#pragma once
#include <vector>
#include <iostream>

class PPU;
class CPU6502;

#include "NES.h"

typedef unsigned char byte;
class Memory {
public:
	Memory();
	void setPPU(PPU& ppu_);
	void setCPU(CPU6502& cpu_);

	//0000 - 07FF = RAM
	//0800 - 1FFF = mirrors of RAM
	//2000 - 2007 = PPU registers accessable from the CPU
	//2008 - 3FFF = mirrors of those same 8 bytes over and over again
	//4000 - 401F = sound channels, joypads, and other IO
	//6000 - 7FFF = cartridge PRG - RAM(if present), or PRG - ROM depending on mapper
	//8000 - FFFF = cartridge memory, usually ROM.
	byte memory[65536]; //the memory itself
	void loadMemory(std::vector<byte> rom, int offset);
	void loadMemory(byte rom[], int romSize, int offset);

	void write(int addr, byte val);
	byte read(int addr);

	//for debug
	bool printReads = false;
	bool printWrites = false;

private:
	CPU6502* cpu;
	PPU* ppu;
};