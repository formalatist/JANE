#pragma once
#include <vector>
#include <iostream>
#include "NES.h"

//typedef for byte
typedef unsigned char byte;

class Memory;

class CPU6502 {
public:
	//constructor
	CPU6502(Memory* memory_);

	//number of cycles per second (6502 is a ~1.7MHz CPU)
	const int CPUFrequency = 1789773;

	//flags
	bool C = false; //carry flag. 1 when overflow or underflow
	bool Z = false; //zero flag. 1 when result is zero
	bool IntDisable = true; //interrupt disable flag. 1 when interrupts are disabled
	bool D = 1; //decimal mode. 1 when decimal mode is active. --THE NES DOES NOT USE THIS--
	bool B = 0; //break flag. 1 when a BRK instruction has been executed and an interrupt has been generated to process it
	bool U = true; //Unused flag, always set
	bool V = false; //overflow flag. 1 when neg + neg = pos or pos + pos = neg in an arithmetic op
	bool N = false; //negative flag. 1 when bit 7 is set in the result of the last op

	//8 bit registers A, X and Y
	unsigned char A = 0;
	unsigned char X = 0;
	unsigned char Y = 0;

	//8 bit stack pointer. Starts at ram adress 0x100 so can adress 0x100-0x1ff
	unsigned char SP = 0xFD;

	//16bit program counter
	unsigned int PC;

	//trigger NMI on the next instruction
	void triggerNMI();

	//the number of cpu cycles we have done
	int cycles = 0;

	//performes one cpu step, returns the number of cycles the step took
	int step();
	//increment this number to make the cpu "stall" for a number of cycles
	//use this to make sure the PPU and CPU are in sync
	int cyclesToStall = 0;
	//set to true if the cpu should halt next step
	bool HALT = false;
	//for debugging
	bool usedOPs[256];
	bool printCallLog = false;
	int numSteps = 0;
	int numImplementedOps = 0;

	//reference to the memory we are using
	Memory* memory;
private:
	//execute the op pointed to by PC
	void executeOP();

	bool NMIWaiting;
	void NMI();

	//push and pull to the stack
	void push(byte high, byte low);
	void pushByte(byte val);
	void push(int word);
	void pushStatus(bool bit4);

	void pullStatus();
	int pullWord();
	byte pullByte();


	//various functions for doing ops
	void NOP(); //the NOP op

	//unimplemented op
	void unimplementedOP();

	//addressing modes
	//read the memory at these locations
	//unsigned char readAccumulator(); not needed since we always have easy access to the A reg
	unsigned char readImmediate();		//reads the next byte
	unsigned char readZeroPage();		//reads the memoryaddress of the next byte
	unsigned char readZeroPageX();		//reads the memoryaddress of the next byte + X
	unsigned char readZeroPageY();		//reads the memoryadress of the next byte + Y
	unsigned char readRelative();		//changes the PC by the value of the next byte
	unsigned char readAbsolute();		//reads memoryaddress of the two next bytes
	unsigned char readAbsoluteX();		//reads the memoryaddress of the two next bytes + X
	unsigned char readAbsoluteY();		//reads the memoryaddress of the two next bytes + Y
	int readIndirect();		//only used by JMP. The instruction contains a 16 bit 
										//address which identifies the location of the least significant 
										//byte of another 16 bit memory address which is the real target 
										//of the instruction.
	unsigned char readIndirectX();		//also called indexed indirect. Indexed indirect addressing is 
										//normally used in conjunction with a table of address held on zero 
										//page. The address of the table is taken from the instruction and 
										//the X register added to it (with zero page wrap around) to give 
										//the location of the least significant byte of the target address.
	unsigned char readIndirectY();		//Indirect indirect addressing is the most common indirection mode 
										//used on the 6502. In instruction contains the zero page location 
										//of the least significant byte of 16 bit address. The Y register 
										//is dynamically added to this value to generated the actual 
										//target address for operation.
	//get the addresses of the different modes without incrementing PC or actually reading the memory
	byte zeroPage();
	byte zeroPageX();
	byte zeroPageY();
	int absolute();
	int absoluteX();
	int absoluteY();
	int indirectX();
	int indirectY();

	int readWord();
	byte readByte();
};