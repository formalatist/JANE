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
