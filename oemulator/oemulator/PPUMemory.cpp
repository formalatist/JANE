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
		mapper->write(addr, val);
	} else if(addr < 0x3000) { //Nametables 0-3 
		/*if (isMirrorVertical) {
			int offset = ((addr > 0x23FF && addr < 0x2800) || addr > 0x2BFF) ? 0x400 : 0;
			addr = ((addr - 0x2000) % 0x400) + offset + 0x2000;
		}
		else { //Horizontal mirror
			int offset = addr > 0x27FF ? 0x800 : 0;
			addr = ((addr - 0x2000) % 0x400) + offset + 0x2000;
		}*/
		addr = getMirroredAddress(addr, mapper->mirrorMode);
		memory[addr] = val;
	} else if (addr < 0x3F00) { //mirrors of them at(0x3000 - 0x3EFF)
		//std::cout << "#######WRITE tO MIRROR OF NAMETABLE" << std::endl;
		addr -= 0x1000;
		/*if (isMirrorVertical) {
			int offset = ((addr > 0x23FF && addr < 0x2800) || addr > 0x2BFF) ? 0x400 : 0;
			addr = ((addr - 0x2000) % 0x400) + offset + 0x2000;
		}
		else { //Horizontal mirror
			int offset = addr > 0x27FF ? 0x800 : 0;
			addr = ((addr - 0x2000) % 0x400) + offset + 0x2000;
		}*/
		addr = getMirroredAddress(addr, mapper->mirrorMode);
		memory[addr] = val;
	} else if (addr < 0x3F20) { //palette table
		//From nesdev: Addresses $3F10/$3F14/$3F18/$3F1C are mirrors of $3F00/$3F04/$3F08/$3F0C
		if (addr == 0x3f10) {
			addr = 0x3f00;
		} else if (addr == 0x3f14) {
			addr = 0x3f04;
		} else if (addr == 0x3f18) {
			addr = 0x3f08;
		} else if (addr == 0x3f1c) {
			addr = 0x3f0c;
		}
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
		//return memory[addr];
		return mapper->read(addr);
	} else if (addr < 0x3000) { //nametables
		/*if (isMirrorVertical) {
			int offset = ((addr > 0x23FF && addr < 0x2800) || addr > 0x2BFF) ? 0x400 : 0;
			addr = ((addr - 0x2000) % 0x400) + offset + 0x2000;
		}
		else { //Horizontal mirror
			int offset = addr > 0x27FF ? 0x800 : 0;
			addr = ((addr - 0x2000) % 0x400) + offset + 0x2000;
		}*/
		addr = getMirroredAddress(addr, mapper->mirrorMode);
		return memory[addr];
	} else if(addr < 0x3F00) { //mirror of nametables
		addr -= 0x1000;
		/*if (isMirrorVertical) {
			int offset = ((addr > 0x23FF && addr < 0x2800) || addr > 0x2BFF) ? 0x400 : 0;
			addr = ((addr - 0x2000) % 0x400) + offset + 0x2000;
		}
		else { //Horizontal mirror
			int offset = addr > 0x27FF ? 0x800 : 0;
			addr = ((addr - 0x2000) % 0x400) + offset + 0x2000;
		}*/
		addr = getMirroredAddress(addr, mapper->mirrorMode);
		return memory[addr];
	} else if(addr < 0x3F20) { //palette
		//std::cout << "PALETTE READ: addr: " << (int)addr << "  val: " << (int)memory[addr] << std::endl;
		//int addrOverRepeatArea = addr - 0x3f00;
		//addr = 0x3f00 + addrOverRepeatArea % 0x10;
		if (addr == 0x3f10) {
			addr = 0x3f00;
		}
		else if (addr == 0x3f14) {
			addr = 0x3f04;
		}
		else if (addr == 0x3f18) {
			addr = 0x3f08;
		}
		else if (addr == 0x3f1c) {
			addr = 0x3f0c;
		}
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

void PPUMemory::setMapper(Mapper* mapper_)
{
	mapper = mapper_;
}

int PPUMemory::getMirroredAddress(int addr, MirrorMode mode)
{
	int intraTableAddr = addr % 0x400;
	int table = (addr - 0x2000) / 0x400; //current table

	//update table based on mirroring mode
	switch (mode)
	{
	case Horizontal:
		if (table == 1) {
			table = 0;
		} else if (table == 2 || table == 3) {
			table = 1;
		}
		break;
	case Vertical:
		if (table == 2) {
			table = 0;
		}
		else if (table == 1 || table == 3) {
			table = 1;
		}
		break;
	case Single0:
		table = 0;
		break;
	case Single1:
		table = 1;
		break;
	case Four:
		break;
	default:
		std::cout << "####### NO MIRROR MODE SET" << std::endl;
		break;
	}
	return 0x2000 + table * 0x400 + intraTableAddr;
}
