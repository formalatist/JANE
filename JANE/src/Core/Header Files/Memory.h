#pragma once
#include <vector>
#include <iostream>

class PPU;
class CPU6502;
class Controller;
class Mapper0;

#include "NES.h"
#include "Mappers/Mapper0.h" //TOTO: MOVE THESE
#include "Mappers/Mapper1.h"
#include "Mappers/Mapper2.h"
#include "Mappers/Mapper3.h"

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
	byte memory[0xffff+1]; //the memory itself. //TODO: this shoul only be 2kb
	void loadMemory(std::vector<byte> rom, int offset);
	void loadMemory(byte rom[], int romSize, int offset);

	void write(int addr, byte val);
	byte read(int addr);

	void connectController(Controller* c1_);

	void setMapper(Mapper* mapper_);

	//for debug
	bool printReads = false;
	bool printWrites = false;

private:
	CPU6502* cpu;
	PPU* ppu;
	Controller* c1;
	Mapper* mapper;
};