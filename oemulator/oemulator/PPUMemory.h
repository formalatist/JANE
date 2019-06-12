#pragma once
#include <vector>
#include <iostream>
#include <iomanip>

class PPU;
class CPU6502;

#include "NES.h"

typedef unsigned char byte;

class PPUMemory {
public:
	PPUMemory();
	void setPPU(PPU& ppu_);
	void setCPU(CPU6502& cpu_);

	//$0000-$0FFF = Pattern Table 0
	//$1000-$1FFF = Pattern Table 1
	//$2000-$23FF = Nametable 0
	//$2400-$27FF = Nametable 1
	//$2800-$2BFF = Nametable 2
	//$2C00-$2FFF = Nametable 3
	//$3000-$3EFF = Mirrors of $2000-$2EFF
	//$3F00-$3F1F = Palette RAM indexes
	//$3F20-$3FFF = Mirrors of $3F00-$3F1F
	byte memory[16384]; //the memory itself
	void loadMemory(std::vector<byte> rom, int offset);
	void loadMemory(byte rom[], int romSize, int offset);

	void write(int addr, byte val);
	byte read(int addr);

	void setMirror(bool mirror);

	//for debug
	bool printReads = false;
	bool printWrites = false;

	//TODO: Refractor. communication should just happen via
	//the NES itself
	//has to be public so that PPU can use its reference
	//to PPUMemory to get to the CPU
	CPU6502* cpu;
private:
	PPU* ppu;
	bool isMirrorVertical;
};