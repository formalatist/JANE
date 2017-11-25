#include "CPU6502.h"

void CPU6502::step()
{
	executeOP();
}

void CPU6502::executeOP()
{
	switch (memory[PC])
	{
	default:
		NOP();
		break;
	}
}

void CPU6502::NOP()
{
	//only increments the PC
	PC++;
}

unsigned char CPU6502::readImmediate()
{
	unsigned char val = memory[PC];
	PC++;
	return val;
}

unsigned char CPU6502::readZeroPage()
{
	unsigned char addr = memory[PC + 1];
	unsigned char val = memory[addr & 0xff];
	PC++;
	return val;
}
