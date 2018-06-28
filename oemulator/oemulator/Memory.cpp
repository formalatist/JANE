#include "Memory.h"

Memory::Memory()
{
}

void Memory::setPPU(PPU & ppu_)
{
	ppu = &ppu_;
}

void Memory::setCPU(CPU6502 & cpu_)
{
	cpu = &cpu_;
}

void Memory::loadMemory(std::vector<byte> rom, int offset)
{
	int memoryI = offset;
	for (std::vector<byte>::iterator i = rom.begin(); i != rom.end(); ++i)
	{
		memory[memoryI] = *i;
		memoryI++;
	}
}

void Memory::loadMemory(byte rom[], int romSize, int offset)
{
	for (int i = 0; i < romSize; i++)
	{
		memory[i + offset] = rom[i];
	}
}

void Memory::write(int addr, byte val)
{
	//0000 - 07FF = RAM
	//0800 - 1FFF = mirrors of RAM
	//2000 - 2007 = PPU registers accessable from the CPU
	//2008 - 3FFF = mirrors of those same 8 bytes over and over again
	//4000 - 401F = sound channels, joypads, and other IO
	//6000 - 7FFF = cartridge PRG - RAM(if present), or PRG - ROM depending on mapper
	//8000 - FFFF = cartridge memory, usually ROM.
	if (addr < 0x2000) { //normal RAM
		addr %= 0x800; //RAM is mirrored after 0x07FF
		memory[addr] = val;
	}
	else if (addr < 0x4000) { //PPU registers
		addr = 0x2000 + (addr % 8); //the 8 PPU registers are 0x2000-0x2007 then repeated
		ppu->writeRegiter(addr, val);
	}
	else {
		std::cout << "Unhandeled write to address: " << addr << ". Value: " << val << std::endl;
	}
}
