#include "Memory.h"
//FOR DEBUG
#include <windows.h>

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
	std::cout << "From Memory.cpp. Loading memory. memory[0] = " << memory[0] << std::endl;
	for (int i = 0; i < romSize; i++)
	{
		//std::cout << "memory[" << i + offset << "] = " << memory[i + offset] << std::endl;
		memory[i + offset] = rom[i];
		//std::cout << "memory[" << i + offset << "] = " << memory[i + offset] << std::endl;
	}
	std::cout << "From Memory.cpp. Loaded memory. memory[0] = " << memory[0] << std::endl;
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
	if (printWrites) {
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
		std::cout << "Write to addr: " << std::hex << addr << " value: " << std::hex << val << std::endl;
	}
}

byte Memory::read(int addr)
{
	if (printReads) {
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
		std::cout << "Read of addr: " << std::hex << addr << std::endl;
	}
	//TODO: Temporary, we need to properly read PPU registers etc
	return memory[addr];
}