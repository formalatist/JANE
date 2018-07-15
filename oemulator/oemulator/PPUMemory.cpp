#include "PPUMemory.h"

PPUMemory::PPUMemory()
{
}

void PPUMemory::setPPU(PPU & ppu_)
{
	ppu = &ppu_;
}

void PPUMemory::setCPU(CPU6502 & cpu_)
{
	cpu = &cpu_;
}

void PPUMemory::loadMemory(std::vector<byte> rom, int offset)
{
	int memoryI = offset;
	for (std::vector<byte>::iterator i = rom.begin(); i != rom.end(); ++i)
	{
		memory[memoryI] = *i;
		memoryI++;
	}
}

void PPUMemory::loadMemory(byte rom[], int romSize, int offset)
{
	std::cout << "From PPUMemory.cpp. Loading memory. memory[0] = " << memory[0] << std::endl;
	for (int i = 0; i < romSize; i++)
	{
		//std::cout << "memory[" << i + offset << "] = " << memory[i + offset] << std::endl;
		memory[i + offset] = rom[i];
		//std::cout << "memory[" << i + offset << "] = " << memory[i + offset] << std::endl;
	}
	std::cout << "From PPUMemory.cpp. Loaded memory. memory[0] = " << memory[0] << std::endl;
}

void PPUMemory::write(int addr, byte val)
{
	addr %= 0x4000;
	if (addr < 0x2000) { //pattern table
		//TODO add mappers that we call here. so we can use CHR RAM if the cartridge has it
		//and other rerouting stuff that mappers do
		//for now just write the CHR ROM from memory directly
		//std::cout << "WRITING TO PATTERN TABLE: " << (int)addr << std::endl;
		//std::cout << "opcode that did it: "
			//<< (int)cpu->memory->read(cpu->PC)
			//<< "  step: " << (int)cpu->numSteps << std::endl;
		//memory[addr] = val;
	} else if(addr < 0x3000) { //Nametables 0-3 
		//std::cout << "Write to nametable: addr: " << (int)addr << "  val: " << (int)val << std::endl;
		if (isMirrorVertical) {
			int offset = ((addr > 0x23FF && addr < 0x2800) || addr > 0x2BFF) ? 0x400 : 0;
			addr = ((addr - 0x2000) % 0x400) + offset + 0x2000;
		}
		else { //Horizontal mirror
			int offset = addr > 0x27FF ? 0x800 : 0;
			addr = ((addr - 0x2000) % 0x400) + offset + 0x2000;
		}
		memory[addr] = val;
	} else if (addr < 0x3F00) { //mirrors of them at(0x3000 - 0x3EFF)
		//std::cout << "#######WRITE tO MIRROR OF NAMETABLE" << std::endl;
		addr -= 0x1000;
		if (isMirrorVertical) {
			int offset = ((addr > 0x23FF && addr < 0x2800) || addr > 0x2BFF) ? 0x400 : 0;
			addr = ((addr - 0x2000) % 0x400) + offset + 0x2000;
		}
		else { //Horizontal mirror
			int offset = addr > 0x27FF ? 0x800 : 0;
			addr = ((addr - 0x2000) % 0x400) + offset + 0x2000;
		}
		memory[addr] = val;
	} else if (addr < 0x3F20) { //palette table
		//std::cout << "PALETTE WRITE: addr: " << (int)addr << "  val: " << (int)val << std::endl;
		memory[addr] = val;
	} else if(addr < 0x4000) { //mirrors of palette
		memory[addr % 0x20 + 0x3F00] = val;
	}
	else {
		std::cout << "opcode that did it: "
			<< (int)cpu->memory->read(cpu->PC) 
			<< "  step: " << (int)cpu->numSteps << std::endl;
		std::cout << "Tried to write to addr: " << addr << " val: " << val << std::endl;
	}
}

byte PPUMemory::read(int addr)
{
	addr %= 0x4000;
	if(addr < 0x2000) { //pattern tables
		return memory[addr];
	} else if (addr < 0x3000) { //nametables
		if (isMirrorVertical) {
			int offset = ((addr > 0x23FF && addr < 0x2800) || addr > 0x2BFF) ? 0x400 : 0;
			addr = ((addr - 0x2000) % 0x400) + offset + 0x2000;
		}
		else { //Horizontal mirror
			int offset = addr > 0x27FF ? 0x800 : 0;
			addr = ((addr - 0x2000) % 0x400) + offset + 0x2000;
		}
		return memory[addr];
	} else if(addr < 0x3F00) { //mirror of nametables
		addr -= 0x1000;
		if (isMirrorVertical) {
			int offset = ((addr > 0x23FF && addr < 0x2800) || addr > 0x2BFF) ? 0x400 : 0;
			addr = ((addr - 0x2000) % 0x400) + offset + 0x2000;
		}
		else { //Horizontal mirror
			int offset = addr > 0x27FF ? 0x800 : 0;
			addr = ((addr - 0x2000) % 0x400) + offset + 0x2000;
		}
		return memory[addr];
	} else if(addr < 0x3F20) { //palette
		//std::cout << "PALETTE READ: addr: " << (int)addr << "  val: " << (int)memory[addr] << std::endl;
		return memory[addr];
	} else if(addr < 0x4000) { //mirrors of palette
		//std::cout << "PALETTE READ: addr: " << (int)addr << "  val: " << (int)memory[addr] << std::endl;
		return memory[addr%0x20 + 0x3F00];
	} else {
		std::cout << "Tried to read addr: " << addr << std::endl;
		return 0;
	}
}

void PPUMemory::setMirror(bool mirror)
{
	isMirrorVertical = mirror;
}
