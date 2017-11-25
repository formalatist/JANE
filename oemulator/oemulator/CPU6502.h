#pragma once

class CPU6502 {
public:
	//8 bit registers A, X and Y
	unsigned char A = 0;
	unsigned char X = 0;
	unsigned char Y = 0;

	//8 bit stack pointer. Starts at ram adress 0x100 so can adress 0x100-0x1ff
	unsigned char SP;

	//16bit program counter
	unsigned int PC;

	//the number of cpu cycles we have done
	int cycles = 0;

	unsigned char memory[65536];

	//performes one cpu step
	void step();

private:
	//execute the op pointed to by PC
	void executeOP();

	//various functions for doing ops
	void NOP(); //the NOP op

};