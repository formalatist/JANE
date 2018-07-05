#include "CPU6502.h"
//FOR DEBUG
#include <windows.h>

CPU6502::CPU6502(Memory* memory_)
{
	memory = memory_;
	//do nothign for now
}


void CPU6502::triggerNMI()
{
	NMIWaiting = true;
}

int CPU6502::step()
{
	if (HALT) {
		return 0;
	}
	numSteps++;
	int cyclesBeforeOP = cycles;
	//if we need to stall cycles just return here
	if (cyclesToStall > 0) {
		cyclesToStall--;
		return 1;
	}

	//check if we have interrupt waiting
	if (NMIWaiting) {
		NMIWaiting = false;
		NMI();
	}
	executeOP();
	//return the number of cycles it took to complete the OP
	return cycles - cyclesBeforeOP;
}

void CPU6502::executeOP()
{
	/*
	if (usedOPs[memory[PC]]) {
		std::cout << "Executing new op: 0x" << std::hex << (int)memory[PC] << std::endl;
		std::cout << "PC: 0x" << std::hex << PC << std::endl;
		usedOPs[memory[PC]] = false;
		numImplementedOps++;
	}
	*/
	if (printCallLog) {
		/*
		std::cout << "Executing op: 0x" << std::hex << (int)memory[PC] << std::endl;
		std::cout << "PC: 0x" << std::hex << PC << std::endl;
		std::cout << "SP: 0x" << std::hex << (int)SP 
			<< "  SP points to: " << std::hex << (int)memory[SP] << "  " << (int)memory[SP + 1] << std::endl;
		std::cout << "Steps: " << numSteps << std::endl;
		std::cout << "\n" << std::endl;
		*/
		int P = C
			| (Z << 1)
			| (IntDisable << 2)
			| (D << 3)
			| (B << 4)
			| (1 << 5)
			| (V << 6)
			| (N << 7);
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10 + (numSteps % 2) * 5);
		std::cout << std::dec << numSteps << std::hex << "   " << "A: " << (int)A << " X: " << (int)X << " Y: " << (int)Y << " SP: "
			<< std::hex << (int)SP << " P: " << std::hex << P << "	$" << std::hex << PC << ":" << (int)memory->memory[PC]
			<< "   " << (int)memory->memory[PC + 1] << std::endl;
	}
	
	switch (memory->memory[PC])
	{
		case 0x0: //BRK force interrupt, implied
		{
			//NOTE: we save PC then flags to stack. so the stack is [flags | PCLO | PCHI]
			push(PC);
			pushStatus(1);
			PC = memory->memory[0xfffe] | (memory->memory[0xffff] << 8);
			B = 1;
			cycles += 7;
			break;
		}
		case 0x1: //ORA logival inclusive OR indirect X
		{
			byte val = A | readIndirectX();
			A = val;
			Z = val == 0;
			N = (val & 0x80) == 0x80;
			cycles += 6;

			break;
		}
		case 0x5: //ORA logical inclusive OR, zero page
		{
			byte val = A | readZeroPage();
			A = val;
			Z = val == 0;
			N = (val & 0x80) == 0x80;
			cycles += 3;

			break;
		}
		case 0x6: //ASL Arithmetic shift left, zero page
		{
			byte val = readZeroPage();
			C = (val & 0x80) == 0x80;
			val = (val << 1) & 0xff;
			Z = val == 0;
			N = (val & 0x80) == 0x80;
			memory->write(memory->memory[PC + 1], val);
			cycles += 5;
			break;
		}
		case 0x8: //PHP push procesor status, implied
		{
			pushStatus(1);
			PC++;
			cycles += 3;
			break;
		}
		case 0x9: //ORA logical inclusive or, immediate
		{
			byte val = A | readImmediate();
			A = val;
			Z = val == 0;
			N = (val & 0x80) == 0x80;
			cycles += 2;
			break;
		}
		case 0xa: //ASL arithmetic shift left, accumulator
		{
			byte val = (A << 1) & 0xff;
			C = (A & 0x80) == 0x80;
			A = val;
			Z = val == 0;
			N = (val & 0x80) == 0x80;
			PC++;
			cycles += 2;
			break;
		}
		case 0xd: //ORA logical inclusive or, absolute
		{
			byte val = A | readAbsolute();
			A = val;
			Z = val == 0;
			N = (val & 0x80) == 0x80;
			cycles += 4;
			break;
		}
		case 0xe: //Arithmetic shift left, absolute
		{
			int addr = readWord();
			byte val = (memory->memory[addr] << 1) & 0xff;
			Z = val == 0;
			N = (val & 0x80) == 0x80;
			C = (memory->memory[addr] & 0x80) == 0x80;
			memory->write(addr, val);
			cycles += 6;
			break;
		}
		case 0x10: //BPL branch if positive
		{
			//NOTE: for all branches we incerment PC before we check the high byte with PCH, this will skew when we detect page changes
			byte val = readImmediate();
			if (!N) {
				byte PCH = (PC & 0xff00) >> 8;
				//treat the value as a signed char
				if ((val & 0x80) == 0x80) { //val is negative
					//invert bits
					val = ~val;
					//add 1
					val++;
					PC -= val;
				}
				else { // val is positive
					PC += val;
				}
				cycles += 3;
				if (PCH != (PC & 0xff00) >> 8) { //new page
					cycles += 2;
				}
			}
			else {
				cycles += 2;
			}
			break;
		}
		case 0x11: //ORA logical inclusive or, indirct Y
		{
			byte val = A | readIndirectY();
			A = val;
			Z = val == 0;
			N = (val & 0x80) == 0x80;
			cycles += 5;
			break;
		}
		case 0x15: //ORA logical inclusive or, zeropage X
		{
			byte val = A | readZeroPageX();
			A = val;
			Z = val == 0;
			N = (val & 0x80) == 0x80;
			cycles += 4;
			break;
		}
		case 0x16: //ASL arithmetic shift left
		{
			byte val = readZeroPageX();
			C = (val & 0x80) == 0x80;
			val = (val << 1) & 0xff;
			Z = val == 0;
			N = (val & 0x80) == 0x80;
			memory->write(zeroPageX(), val);
			cycles += 6;
			break;
		}
		case 0x18: //CLC clear carry flag
		{
			C = 0;
			cycles += 2;
			PC++;
			break;
		}
		case 0x19: //ORA logical inclusive or, absoluteY
		{
			byte val = A | readAbsoluteY();
			A = val;
			Z = val == 0;
			N = (val & 0x80) == 0x80;
			cycles += 4;
			break;
		}
		case 0x1d: //ORA logival incluive or, absoluteX
		{
			byte val = A | readAbsoluteX();
			A = val;
			Z = val == 0;
			N = (val & 0x80) == 0x80;
			cycles += 4;
			break;
		}
		case 0x1e: //ASL arithmetic shift left, abosluteX
		{
			byte val = readAbsoluteX();
			C = (val & 0x80) == 0x80;
			val = (val << 1) & 0xff;
			Z = val == 0;
			N = (val & 0x80) == 0x80;
			memory->write(absoluteX(), val);
			cycles += 7;
			break;
		}
		case 0x20: //JSR jump to subroutine, absolute
		{
			//TODO this might be the wrong PC to push
			push(PC + 2);
			PC = readWord();
			cycles += 6;
			break;
		}
		case 0x21: //AND logical and, indirectX
		{
			byte val = A & readIndirectX();
			A = val;
			Z = val == 0;
			N = (val & 0x80) == 0x80;
			cycles += 6;
			break;
		}
		case 0x24: //BIT bit test, zeroPage
		{
			byte val = readZeroPage();
			Z = (A & val) == 0;
			V = (val & 0x40) == 0x40;
			N = (val & 0x80) == 0x80;
			cycles += 3;
			break;
		}
		case 0x25: //AND logical and, zeroPage
		{
			byte val = A & readZeroPage();
			A = val;
			Z = val == 0;
			N = (val & 0x80) == 0x80;
			cycles += 3;
			break;
		}
		case 0x26: //ROL Rotate left, zeroPage
		{
			byte val = readZeroPage();
			bool tempC = C;
			C = (val & 0x80) == 0x80;
			val = ((val << 1) | tempC) & 0xff;
			N = (val & 0x80) == 0x80;
			Z = val == 0;
			memory->write(zeroPage(), val);
			cycles += 5;
			break;
		}
		case 0x28: //PLP pull processor status
		{
			pullStatus();
			PC++;
			cycles += 4;
			break;
		}
		case 0x29: //AND logical and, immediate
		{
			byte val = readImmediate() & A;
			A = val;
			Z = val == 0;
			N = (val & 0x80) == 0x80;
			cycles += 2;

			break;
		}
		case 0x2a: //ROL rotate left, Accumulator
		{
			bool tempC = C;
			C = (A & 0x80) == 0x80;
			A = ((A << 1) | tempC) & 0xff;
			N = (A & 0x80) == 0x80;
			Z = A == 0;
			PC++;
			cycles += 2;
			break;
		}
		case 0x2c: //BIT bit test, absolute
		{
			byte val = readAbsolute();
			Z = (A & val) == 0;
			N = (val & 0x80) == 0x80;
			V = (val & 0x40) == 0x40;
			cycles += 4;
			break;
		}
		case 0x2d: //AND logical and, absolute
		{
			byte val = A & readAbsolute();
			A = val;
			Z = val == 0;
			N = (val & 0x80) == 0x80;
			cycles += 4;
			break;
		}
		case 0x2e: //ROL rotate left, Absolute
		{
			int addr = readWord();
			byte val = memory->memory[addr];
			bool tempC = C;
			C = (val & 0x80) == 0x80;
			val = ((val << 1) | tempC) & 0xff;
			N = (val & 0x80) == 0x80;
			Z = val == 0;
			memory->write(addr, val);
			cycles += 6;
			break;
		}
		case 0x30: //BMI Branch if minus
		{
			byte val = readRelative();
			if (N) {
				byte PCH = (PC & 0xff00) >> 8;
				//treat the value as a signed char
				if ((val & 0x80) == 0x80) { //val is negative
										  //invert bits
					val = ~val;
					//add 1
					val++;
					PC -= val;
				}
				else { // val is positive
					PC += val;
				}
				cycles += 3;
				if (PCH != (PC & 0xff00) >> 8) { //new page
					cycles += 2;
				}
			}
			else {
				cycles += 2;
			}
			break;
		}
		case 0x31: //AND logical and, indirectY
		{
			byte val = A & readIndirectY();
			A = val;
			Z = val == 0;
			N = (val & 0x80) == 0x80;
			cycles += 5;
			break;
		}
		case 0x35: //AND logical and, ZeroPageX
		{
			byte val = A & readZeroPageX();
			A = val;
			Z = val == 0;
			N = (val & 0x80) == 0x80;
			cycles += 4;
			break;
		}
		case 0x36: //ROL rotate left, zeroPageX
		{
			byte val = readZeroPageX();
			bool tempC = C;
			C = (val & 0x80) == 0x80;
			val = ((val << 1) | tempC) & 0xff;
			N = (val & 0x80) == 0x80;
			Z = val == 0;
			memory->write(zeroPageX(), val);
			cycles += 6;
			break;
		}
		case 0x38: //SEC set carry flag
		{
			C = 1;
			PC++;
			cycles += 2;
			break;
		}
		case 0x39: //AND logical and, AbosluteY
		{
			byte val = A & readAbsoluteY();
			A = val;
			Z = val == 0;
			N = (val & 0x80) == 0x80;
			cycles += 4;
			break;
		}
		case 0x3d: //AND logical and, abosluteX
		{
			byte val = A & readAbsoluteX();
			A = val;
			Z = val == 0;
			N = (val & 0x80) == 0x80;
			cycles += 4;
			break;
		}
		case 0x3e: //ROL rotate left, AbosluteX
		{
			byte val = readAbsoluteX();
			bool tempC = C;
			C = (val & 0x80) == 0x80;
			val = ((val << 1) | tempC) & 0xff;
			N = (val & 0x80) == 0x80;
			Z = val == 0;
			memory->write(absoluteX(), val);
			cycles += 6;
			break;
		}
		case 0x40: //RTO return from interrupt
		{
			pullStatus();
			PC = pullWord();
			cycles += 6;
			break;
		}
		case 0x41: //EOR exclusive or, indirectX
		{
			byte val = A ^ readIndirectX();
			A = val;
			Z = val == 0;
			N = (val & 0x80) == 0x80;
			cycles += 6;
			break;
		}
		case 0x45: //EOR exclusive or, zeroPage
		{
			byte val = A ^ readZeroPage();
			A = val;
			Z = val == 0;
			N = (val & 0x80) == 0x80;
			cycles += 3;
			break;
		}
		case 0x46: //LSR logical shift right, zeroPage
		{
			byte val = readZeroPage();
			C = val & 1;
			val = (val >> 1) & 0xff;
			Z = val == 0;
			N = (val & 0x80) == 0x80;
			memory->write(zeroPage(), val);
			cycles += 5;
			break;
		}
		case 0x48: //PHA push accumulator
		{
			pushByte(A);
			PC++;
			cycles += 3;
			break;
		}
		case 0x49: //EOR exclusive or, immediate
		{
			byte val = A ^ readImmediate();
			A = val;
			Z = val == 0;
			N = (val & 0x80) == 0x80;
			cycles += 2;
			break;
		}
		case 0x4a: //LSR logical shift right, Accumulator
		{
			byte val = A;
			C = val & 1;
			val = (val >> 1) & 0xff;
			Z = val == 0;
			N = (val & 0x80) == 0x80;
			A = val;
			PC++;
			cycles += 5;
			break;
		}
		case 0x4c: //JMP jump
		{
			//TODO the original 6502 has a bug when getting adresses from ends of pages
			//IE 0xXXFF. This might need to be emulated
			PC = readWord();
			cycles += 3;
			break;
		}
		case 0x4d: //EOR exclusive or, absolute
		{
			byte val = A ^ readAbsolute();
			A = val;
			Z = val == 0;
			N = (val & 0x80) == 0x80;
			cycles += 4;
			break;
		}
		case 0x4e: //LSR logical shift right, absolute
		{
			byte val = readAbsolute();
			C = val & 1;
			val = (val >> 1) & 0xff;
			Z = val == 0;
			N = (val & 0x80) == 0x80;
			memory->write(absolute(), val);
			cycles += 6;
			break;
		}
		case 0x50:
		{
			byte val = readRelative();
			if (!V) {
				byte PCH = (PC & 0xff00) >> 8;
				//treat the value as a signed char
				if (val & 0x80 == 0x80) { //val is negative
										  //invert bits
					val = ~val;
					//add 1
					val++;
					PC -= val;
				}
				else { // val is positive
					PC += val;
				}
				cycles += 3;
				if (PCH != (PC & 0xff00) >> 8) { //new page
					cycles += 2;
				}
			}
			else {
				cycles += 2;
			}
			break;
		}
		case 0x51: //EOR exclusive or, indirectY
		{
			byte val = A ^ readIndirectY();
			A = val;
			Z = val == 0;
			N = (val & 0x80) == 0x80;
			cycles += 5;
			break;
		}
		case 0x55: //EOR exclusive or, zeroPageX
		{
			byte val = A ^ readZeroPageX();
			A = val;
			Z = val == 0;
			N = (val & 0x80) == 0x80;
			cycles += 4;
			break;
		}
		case 0x56: //LSR logical shift right, zeroPageX
		{
			byte val = readZeroPageX();
			C = val & 1;
			val = (val >> 1) & 0xff;
			Z = val == 0;
			N = (val & 0x80) == 0x80;
			memory->write(zeroPageX(), val);
			cycles += 6;
			break;
		}
		case 0x58: //CLI clear interrupt disable
		{
			IntDisable = 0;
			PC++;
			cycles += 2;
			break;
		}
		case 0x59: //EOR exclusive or, absoluteY
		{
			byte val = A ^ readAbsoluteY();
			A = val;
			Z = val == 0;
			N = (val & 0x80) == 0x80;
			cycles += 4;
			break;
		}
		case 0x5d: //EOR exclusive or, absoluteX
		{
			byte val = A ^ readAbsoluteX();
			A = val;
			Z = val == 0;
			N = (val & 0x80) == 0x80;
			cycles += 4;
			break;
		}
		case 0x5e: //LSR logical shift right, absolueX
		{
			byte val = readAbsoluteX();
			C = val & 1;
			val = (val >> 1) & 0xff;
			Z = val == 0;
			N = (val & 0x80) == 0x80;
			memory->write(absoluteX(), val);
			cycles += 7;
			break;
		}
		case 0x60: //RTS return from subroutine
		{
			PC = pullWord();
			PC++;
			cycles += 6;
			break;
		}
		case 0x61: //ADC add with carry, indirectX
		{
			byte M = readIndirectX();
			int val = A + M + C;
			Z = (val & 0xFF) == 0;
			C = val > 0xff;
			N = (val & 0x80) == 0x80;
			V = (((A ^ val) & (M ^ val)) & 0x80) == 0x80;
			A = val & 0xff;
			cycles += 6;
			break;
		}
		case 0x65: //ADC add with carry, zeroPAge
		{
			byte M = readZeroPage();
			int val = A + M + C;
			Z = (val & 0xFF) == 0;
			C = val > 0xff;
			N = (val & 0x80) == 0x80;
			V = (((A ^ val) & (M ^ val)) & 0x80) == 0x80;
			A = val & 0xff;
			cycles += 3;
			break;
		}
		case 0x66: //ROR rotate right, zeroPAge
		{
			byte val = readZeroPage();
			bool tempC = C;
			C = val & 1;
			val = (val >> 1) | (tempC << 7);
			N = (val & 0x80) == 0x80;
			memory->write(zeroPage(), val);
			cycles += 5;
			break;
		}
		case 0x68: //PLA pull accumulator
		{
			A = pullByte();
			Z = A == 0;
			N = (A & 0x80) == 0x80;
			cycles += 4;
			break;
		}
		case 0x69: //ADC add with carry, immediate
		{
			byte M = readImmediate();
			int val = A + M + C;
			Z = (val & 0xFF) == 0;
			C = val > 0xff;
			N = (val & 0x80) == 0x80;
			V = (((A ^ val) & (M ^ val)) & 0x80) == 0x80;
			A = val & 0xff;
			cycles += 2;
			break;
		}
		case 0x6a: //ROR rotate right
		{
			byte val = A;
			bool tempC = C;
			C = val & 1;
			val = (val >> 1) | (tempC << 7);
			N = (val & 0x80) == 0x80;
			Z = A == 0;
			A = val & 0xff;

			PC++;
			cycles += 2;
			break;
		}
		case 0x6c: //JMP jump, indirect
		{
			PC = readIndirect();
			cycles += 5;
			break;
		}
		case 0x6d: //ADC add with carry, absolute
		{
			byte M = readAbsolute();
			int val = A + M + C;
			Z = (val & 0xFF) == 0;
			C = val > 0xff;
			N = (val & 0x80) == 0x80;
			V = (((A ^ val) & (M ^ val)) & 0x80) == 0x80;
			A = val & 0xff;
			cycles += 4;
			break;
		}
		case 0x6e: //ROR rotate right, absolute
		{
			byte val = readAbsolute();
			bool tempC = C;
			C = val & 1;
			val = (val >> 1) | (tempC << 7);
			N = (val & 0x80) == 0x80;
			Z = A == 0; //this might not be correct for this instruction
			memory->write(absolute(), val);
			cycles += 2;
			break;
		}
		case 0x70: //BVS branch if overflow set
		{
			byte val = readRelative();
			if (V) {
				byte PCH = (PC & 0xff00) >> 8;
				//treat the value as a signed char
				if ((val & 0x80) == 0x80) { //val is negative
										  //invert bits
					val = ~val;
					//add 1
					val++;
					PC -= val;
				}
				else { // val is positive
					PC += val;
				}
				cycles += 3;
				if (PCH != (PC & 0xff00) >> 8) { //new page
					cycles += 2;
				}
			}
			else {
				cycles += 2;
			}
			break;
		}
		case 0x71: //ADC add with carry, indirectY
		{
			byte M = readIndirectY();
			int val = A + M + C;
			Z = (val & 0xFF) == 0;
			C = val > 0xff;
			N = (val & 0x80) == 0x80;
			V = (((A ^ val) & (M ^ val)) & 0x80) == 0x80;
			A = val & 0xff;
			cycles += 5;
			break;
		}
		case 0x75: //ADC add with carry, zeroPageX
		{
			byte M = readZeroPageX();
			int val = A + M + C;
			Z = (val & 0xFF) == 0;
			C = val > 0xff;
			N = (val & 0x80) == 0x80;
			V = (((A ^ val) & (M ^ val)) & 0x80) == 0x80;
			A = val & 0xff;
			cycles += 4;
			break;
		}
		case 0x76: //ROR rotate right, zeroPageX
		{
			byte val = readZeroPageX();
			bool tempC = C;
			C = val & 1;
			val = (val >> 1) | (tempC << 7);
			N = (val & 0x80) == 0x80;
			Z = A == 0; //this might not be correct for this instruction
			memory->write(zeroPageX(), val);
			cycles += 6;
			break;
		}
		case 0x78: //SEI - Set interrupt disable, implied
		{
			IntDisable = 1;
			PC++;
			cycles += 2;
			break;
		}
		case 0x79: //ADC add with carry, absoluteY
		{
			byte M = readAbsoluteY();
			int val = A + M + C;
			Z = (val & 0xFF) == 0;
			C = val > 0xff;
			N = (val & 0x80) == 0x80;
			V = (((A ^ val) & (M ^ val)) & 0x80) == 0x80;
			A = val & 0xff;
			cycles += 4;
			break;
		}
		case 0x7d: //ADC add with carry, absoluteX
		{
			byte M = readAbsoluteX();
			int val = A + M + C;
			Z = (val & 0xFF) == 0;
			C = val > 0xff;
			N = (val & 0x80) == 0x80;
			V = (((A ^ val) & (M ^ val)) & 0x80) == 0x80;
			A = val & 0xff;
			cycles += 4;
			break;
		}
		case 0x7e: //ROR rotate right, absoluteX
		{
			byte val = readAbsoluteX();
			bool tempC = C;
			C = val & 1;
			val = (val >> 1) | (tempC << 7);
			N = (val & 0x80) == 0x80;
			Z = A == 0; //this might not be correct for this instruction
			memory->write(absoluteX(), val);
			cycles += 7;
			break;
		}
		case 0x81: //STA store accumulator, indirectX
		{
			memory->write(indirectX(), A);
			PC += 2;
			cycles += 6;
			break;
		}
		case 0x84: //STY store Y register, zeroPAge
		{
			memory->write(zeroPage(), Y);
			PC += 2;
			cycles += 3;
			break;
		}
		case 0x85: //STA store accumulator, zeroPage
		{
			memory->write(zeroPage(), A);
			PC += 2;
			cycles += 3;
			break;
		}
		case 0x86: //STX store X register, zeroPage
		{
			memory->write(zeroPage(), X);
			PC += 2;
			cycles += 3;
			break;
		}
		case 0x88: //DEY decrement Y register
		{
			Y = (Y - 1) & 0xff;
			Z = Y == 0;
			N = (Y & 0x80) == 0x80;
			PC++;
			cycles += 2;
			break;
		}
		case 0x8a: //TXA transfer X to Accumulator
		{
			A = X;
			Z = A == 0;
			N = (A & 0x80) == 0x80;
			PC++;
			cycles += 2;
			break;
		}
		case 0x8c: //STY store Y register, absolute
		{
			memory->write(absolute(), Y);
			PC += 3;
			cycles += 4;
			break;
		}
		case 0x8d: //STA store accumulator, absolute
		{
			memory->write(absolute(), A);
			PC += 3;
			cycles += 4;
			break;
		}
		case 0x8e: //STX store X register, absolute
		{
			memory->write(absolute(), X);
			PC += 3;
			cycles += 4;
			break;
		}
		case 0x90: //BCC branch if carry clear
		{
			byte val = readImmediate();
			if (!C) {
				byte PCH = (PC & 0xff00) >> 8;
				//treat the value as a signed char
				if ((val & 0x80) == 0x80) { //val is negative
										  //invert bits
					val = ~val;
					//add 1
					val++;
					PC -= val;
				}
				else { // val is positive
					PC += val;
				}
				cycles += 3;
				if (PCH != (PC & 0xff00) >> 8) { //new page
					cycles += 2;
				}
			}
			else {
				cycles += 2;
			}
			break;
		}
		case 0x91: //STA store accumulator, indirectY
		{
			memory->write(indirectY(), A);
			PC += 2;
			cycles += 6;
			break;
		}
		case 0x94: //STY store Y register, zeroPageX
		{
			memory->write(zeroPageX(), Y);
			PC += 2;
			cycles += 4;
			break;
		}
		case 0x95: //STA store accumulator, zeroPageX
		{
			memory->write(zeroPageX(), A);
			PC += 2;
			cycles += 4;
			break;
		}
		case 0x96: //STX store X register, zeroPageY
		{
			memory->write(zeroPageY(), X);
			PC += 2;
			cycles += 4;
			break;
		}
		case 0x98: //TYA transfer Y to accumulator
		{
			A = Y;
			Z = A == 0;
			N = (A & 0x80) == 0x80;
			PC++;
			cycles += 2;
			break;
		}
		case 0x99: //STA store accumulator, absoluteY
		{
			memory->write(absoluteY(), A);
			PC += 3;
			cycles += 5;
			break;
		}
		case 0x9a: //TXS Transfer x to stack pointer
		{
			SP = X;
			PC++;
			cycles += 2;

			break;
		}
		case 0x9d: //STA store accumulator, absoluteX
		{
			memory->write(absoluteX(), A);
			PC += 3;
			cycles += 5;
			break;
		}
		case 0xa0: //LDY load Y register, immediate
		{
			Y = readImmediate();
			Z = Y == 0;
			N = (Y & 0x80) == 0x80;
			cycles += 2;
			break;
		}
		case 0xa1: //LDA load accumulator, indirectX
		{
			A = readIndirectX();
			Z = A == 0;
			N = (A & 0x80) == 0x80;
			cycles += 6;
			break;
		}
		case 0xa2: //LDX load x register
		{
			X = readImmediate();
			Z = X == 0;
			N = (X & 0x80) == 0x80;
			cycles += 2;

			break;
		}
		case 0xa4: //LDY load Y register, zeroPage
		{
			Y = readZeroPage();
			Z = Y == 0;
			N = (Y & 0x80) == 0x80;
			cycles += 3;
			break;
		}
		case 0xa5: //LDA load accumulator, zeroPage
		{
			A = readZeroPage();
			Z = A == 0;
			N = (A & 0x80) == 0x80;
			cycles += 3;

			break;
		}
		case 0xa6: //LDX load X register, zeroPage
		{
			X = readZeroPage();
			Z = X == 0;
			N = (X & 0x80) == 0x80;
			cycles += 3;
			break;
		}
		case 0xa8: //TAY ytansfer accumulator to Y
		{
			Y = A;
			Z = Y == 0;
			N = (Y & 0x80) == 0x80;
			PC++;
			cycles += 2;
			break;
		}
		case 0xa9: //LDA load accumulator, immediate
		{
			A = readImmediate();
			Z = A == 0;
			N = (A & 0x80) == 0x80;
			cycles += 2;
			break;
		}
		case 0xaa: //TAX transfer accumulator to X
		{
			X = A;
			Z = X == 0;
			N = (X & 0x80) == 0x80;
			cycles += 2;
			PC++;
			break;
		}
		case 0xac: //LDY load Y register, absolute
		{
			Y = readAbsolute();
			Z = Y == 0;
			N = (Y & 0x80) == 0x80;
			cycles += 4;
			break;
		}
		case 0xad: //LDA load accumulator, absolute
		{
			A = readAbsolute();
			Z = A == 0;
			N = (A & 0x80) == 0x80;
			cycles += 4;
			break;
		}
		case 0xae: //LDX load X register, absolute
		{
			X = readAbsolute();
			Z = X == 0;
			N = (X & 0x80) == 0x80;
			cycles += 4;
			break;
		}
		case 0xb0: //BCS branch if carry set, relative
		{
			byte val = readRelative();
			if (C) {
				byte PCH = (PC & 0xff00) >> 8;
				//treat the value as a signed char
				if ((val & 0x80) == 0x80) { //val is negative
										  //invert bits
					val = ~val;
					//add 1
					val++;
					PC -= val;
				}
				else { // val is positive
					PC += val;
				}
				cycles += 3;
				if (PCH != (PC & 0xff00) >> 8) { //new page
					cycles += 2;
				}
			}
			else {
				cycles += 2;
			}
			break;
		}
		case 0xb1: //LDA load accumulator, indirectY
		{
			A = readIndirectY();
			Z = A == 0;
			N = (A & 0x80) == 0x80;
			cycles += 5;
			break;
		}
		case 0xb4: //LDY load Y register, zeroPageX
		{
			Y = readZeroPageX();
			Z = Y == 0;
			N = (Y & 0x80) == 0x80;
			cycles += 4;
			break;
		}
		case 0xb5: //LDA load accumulator, zeroPageX
		{
			A = readZeroPageX();
			Z = A == 0;
			N = (A & 0x80) == 0x80;
			cycles += 4;
			break;
		}
		case 0xb6: //LDX load X register, zeroPageY
		{
			X = readZeroPageY();
			Z = X == 0;
			N = (X & 0x80) == 0x80;
			cycles += 4;
			break;
		}
		case 0xb8: //CLV clear overflow flag
		{
			V = 0;
			cycles += 2;
			PC++;
			break;
		}
		case 0xb9: //LDA load accumulator, absoluteY
		{
			A = readAbsoluteY();
			Z = A == 0;
			N = (A & 0x80) == 0x80;
			cycles += 4;
			break;
		}
		case 0xba: //TSX transfer stack pointer to X
		{
			X = SP;
			Z = X == 0;
			N = (X & 0x80) == 0x80;
			PC++;
			cycles += 2;
			break;
		}
		case 0xbc: //LDY load Y register, absoluteX
		{
			Y = readAbsoluteX();
			Z = Y == 0;
			N = (Y & 0x80) == 0x80;
			cycles += 4;
			break;
		}
		case 0xbd: //LDA load accumulator, abosluteX
		{
			A = readAbsoluteX();
			Z = A == 0;
			N = (A & 0x80) == 0x80;
			cycles += 4;
			break;
		}
		case 0xbe: //LDX load X register, absoluteY
		{
			X = readAbsoluteY();
			Z = X == 0;
			N = (X & 0x80) == 0x80;
			cycles += 4;
			break;
		}
		case 0xc0: //CPY compare Y register, immediate
		{
			byte val = readImmediate();
			C = Y >= val;
			Z = Y == val;
			N = ((Y - val) & 0x80) == 0x80;
			cycles += 2;
			break;
		}
		case 0xc1: //CMP compare, indirectX
		{
			byte val = readIndirectX();
			C = A >= val;
			Z = A == val;
			N = ((A - val) & 0x80) == 0x80;
			cycles += 6;
			break;
		}
		case 0xc4: //CPY compare Y register, zeroPage
		{
			byte val = readZeroPage();
			C = Y >= val;
			Z = Y == val;
			N = ((Y - val) & 0x80) == 0x80;
			cycles += 3;
			break;
		}
		case 0xc5: //CMP compare, zeroPage
		{
			byte val = readZeroPage();
			C = A >= val;
			Z = A == val;
			N = ((A - val) & 0x80) == 0x80;
			cycles += 3;
			break;
		}
		case 0xc6: //DEC decrement memory, zeroPage
		{
			int addr = zeroPage();
			byte val = readZeroPage();
			val -= 1;
			Z = val == 0;
			N = (val & 0x80) == 0x80;
			memory->write(addr, val);
			cycles += 5;
			break;
		}
		case 0xc8: //INY increment Y register
		{
			Y = Y + 1;
			Z = Y == 0;
			N = (Y & 0x80) == 0x80;
			cycles += 2;
			PC++;
			break;
		}
		case 0xc9: //CMP Compare Immediate
		{
			byte val = readImmediate();
			byte result = A - val;
			C = A >= val;
			Z = A == val;
			N = (result & 0x80) == 0x80;
			cycles += 2;
			break;
		}
		case 0xca: //DEX decrement X register
		{
			X = X - 1;
			Z = X == 0;
			N = (X & 0x80) == 0x80;
			cycles += 2;
			PC++;
			break;
		}
		case 0xcc: //CPY Compare Y register, Aboslute
		{
			byte val = readAbsolute(); 
			byte result = Y - val;
			Z = (result == 0); 
			C = (Y >= val);
			N = (result & 0x80) == 0x80; 
			cycles += 4;
			break;
		}
		case 0xcd: //CMP comare, aboslute
		{
			byte val = readAbsolute();
			C = A >= val;
			Z = A == val;
			N = ((A - val) & 0x80) == 0x80;
			cycles += 4;
			break;
		}
		case 0xce: //DEC decrement memory, absolute
		{
			int addr = absolute();
			byte val = readAbsolute() - 1;
			Z = val == 0;
			N = (val & 0x80) == 0x80;
			memory->write(addr, val);
			cycles += 6;
			break;
		}
		case 0xd0: //BNE Branch of not equal
		{
			byte val = readRelative();
			if (!Z) {
				byte PCH = (PC & 0xff00) >> 8;
				//treat the value as a signed char
				if ((val & 0x80) == 0x80) { //val is negative
										  //invert bits
					val = ~val;
					//add 1
					val++;
					PC -= val;
				}
				else { // val is positive
					PC += val;
				}
				cycles += 3;
				if(PCH != (PC & 0xff00) >> 8) { //new page
					cycles += 2;
				}
			}
			else {
				cycles += 2;
			}
			break;
		}
		case 0xd1: //CMP indirectY
		{
			byte val = readIndirectY();
			C = A >= val;
			Z = A == val;
			N = ((A - val) & 0x80) == 0x80;
			cycles += 5;
			break;
		}
		case 0xd5: //CMP compare, zeroPageX
		{
			byte val = readZeroPageX();
			C = A >= val;
			Z = A == val;
			N = ((A - val) & 0x80) == 0x80;
			cycles += 4;
			break;
		}
		case 0xd6: //DEC decrement memory, zeroPageX
		{
			int addr = zeroPageX();
			byte val = readZeroPageX() - 1;
			Z = val == 0;
			N = (val & 0x80) == 0x80;
			memory->write(addr, val);
			cycles += 6;
			break;
		}
		case 0xd8: //CLD clear decimal mode
		{
			//decimal mode not used by the nes. but the flag can be used to store info
			D = 0;
			PC++;
			cycles += 2;
			break;
		}
		case 0xd9: //CMP compare, absoluteY
		{
			byte val = readAbsoluteY();
			C = A >= val;
			Z = A == val;
			N = ((A - val) & 0x80) == 0x80;
			cycles += 4;
			break;
		}
		case 0xdd: //CMP compare, absoluteX
		{
			byte val = readAbsoluteX();
			C = A >= val;
			Z = A == val;
			N = ((A - val) & 0x80) == 0x80;
			cycles += 4;
			break;
		}
		case 0xde: //DEC decrement memory, absoluteX
		{
			int addr = absoluteX();
			byte val = readAbsoluteX() - 1;
			Z = val == 0;
			N = (val & 0x80) == 0x80;
			memory->write(addr, val);
			cycles += 7;
			break;
		}
		case 0xe0: //CPX compare X register, immediate
		{
			byte val = readImmediate();
			C = X >= val;
			Z = X == val;
			N = ((X - val) & 0x80) == 0x80;
			cycles += 2;
			break;
		}
		case 0xe1: //SBC subtract with carry, indirectX
		{
			//TODO: check this, might be wrong way to do SBC
			byte M = readIndirectX();
			short val = A - M - (1 - C);
			C = (val >= 0);
			Z = (val & 0xff) == 0;
			N = ((val & 0xff) & 0x80) == 0x80;
			V = ((A ^ M) & 0x80) != 0 && ((A^val) & 0x80) != 0;
			A = val & 0xff;
			cycles += 6;
			break;
		}
		case 0xe4: //CPX compare X register, zeroPage
		{
			byte val = readZeroPage();
			C = X >= val;
			Z = X == val;
			N = ((X - val) & 0x80) == 0x80;
			cycles += 3;
			break;
		}
		case 0xe5: //SBC subtract with carry, zeroPage
		{
			byte M = readZeroPage();
			short val = A - M - (1 - C);
			C = (val >= 0);
			Z = (val & 0xff) == 0;
			N = ((val & 0xff) & 0x80) == 0x80;
			V = ((A ^ M) & 0x80) != 0 && ((A^val) & 0x80) != 0;
			A = val & 0xff;
			cycles += 3;
			break;
		}
		case 0xe6: //INC increment memory, zeroPage
		{
			int zeroPageAddr = zeroPage();
			byte val = (readZeroPage() + 1) & 0xff;
			Z = val == 0;
			N = (val & 0x80) == 0x80;
			memory->write(zeroPageAddr, val);
			cycles += 5;
			break;
		}
		case 0xe8: //INX increment X register
		{
			X = X + 1;
			Z = X == 0;
			N = (X & 0x80) == 0x80;
			cycles += 2;
			PC++;
			break;
		}
		case 0xe9: //SBC, subtract with carry, Immediate
		{	
			byte M = readImmediate();
			short val = A - M - (1 - C);
			C = (val >= 0);
			Z = (val & 0xff) == 0;
			N = ((val & 0xff) & 0x80) == 0x80;
			V = ((A ^ M) & 0x80) != 0 && ((A^val) & 0x80) != 0;
			A = val & 0xff;
			cycles += 2;
			break;
		}
		case 0xea: //NOP
		{
			NOP();
			cycles += 2;
			break;
		}
		case 0xec: //CPX compare X register, absolute
		{
			byte val = readAbsolute();
			C = X >= val;
			Z = X == val;
			N = ((X - val) & 0x80) == 0x80;
			cycles += 4;
			break;
		}
		case 0xed: //SBC subtract with carry, absolute
		{
			byte M = readAbsolute();
			short val = A - M - (1 - C);
			C = (val >= 0);
			Z = (val & 0xff) == 0;
			N = ((val & 0xff) & 0x80) == 0x80;
			V = ((A ^ M) & 0x80) != 0 && ((A^val) & 0x80) != 0;
			A = val & 0xff;
			cycles += 4;
			break;
		}
		case 0xee: //INC increment memory, absolute
		{
			int absoluteAddr = absolute();
			byte val = readAbsolute() + 1;
			Z = val == 0;
			N = (val & 0x80) == 0x80;
			memory->write(absoluteAddr, val);
			cycles += 6;
			break;
		}
		case 0xf0: //BEQ Branch if equal. (counter to d0)
		{
			byte val = readRelative();
			if (Z) {
				byte PCH = (PC & 0xff00) >> 8;
				//treat the value as a signed char
				if((val & 0x80) == 0x80) { //val is negative
					//invert bits
					val = ~val;
					//add 1
					val++;
					PC -= val;
				} else { // val is positive
					PC += val;
				}
				cycles += 3;
				if (PCH != (PC & 0xff00) >> 8) { //new page
					cycles += 2;
				}
			}
			else {
				cycles += 2;
			}
			break;
		}
		case 0xf1: //SBC subtract with carry, indirectY
		{
			byte M = readIndirectY();
			short val = A - M - (1 - C);
			C = (val >= 0);
			Z = (val & 0xff) == 0;
			N = ((val & 0xff) & 0x80) == 0x80;
			V = ((A ^ M) & 0x80) != 0 && ((A^val) & 0x80) != 0;
			A = val & 0xff;
			cycles += 5;
			break;
		}
		case 0xf5: //SBC subtract with carry, zeroPageX
		{
			byte M = readZeroPageX();
			short val = A - M - (1 - C);
			C = (val >= 0);
			Z = (val & 0xff) == 0;
			N = ((val & 0xff) & 0x80) == 0x80;
			V = ((A ^ M) & 0x80) != 0 && ((A^val) & 0x80) != 0;
			A = val & 0xff;
			cycles += 4;
			break;
		}
		case 0xf6: //INC increment memory, zeroPageX
		{
			int zeroPageXAddr = zeroPageX();
			byte val = readZeroPageX() + 1;
			Z = val == 0;
			N = (val & 0x80) == 0x80;
			memory->write(zeroPageXAddr, val);
			cycles += 6;
			break;
		}
		case 0xf8: //SED set decimal flag
		{
			D = 1;
			PC++;
			cycles += 2;
			break;
		}
		case 0xf9: //SBC subtract with carry, absoluteY
		{
			byte M = readAbsoluteY();
			short val = A - M - (1 - C);
			C = (val >= 0);
			Z = (val & 0xff) == 0;
			N = ((val & 0xff) & 0x80) == 0x80;
			V = ((A ^ M) & 0x80) != 0 && ((A^val) & 0x80) != 0;
			A = val & 0xff;
			cycles += 4;
			break;
		}
		case 0xfd: //SBC subtract with carry, absoluteX
		{
			byte M = readAbsoluteX();
			short val = A - M - (1 - C);
			C = (val >= 0);
			Z = (val & 0xff) == 0;
			N = ((val & 0xff) & 0x80) == 0x80;
			V = ((A ^ M) & 0x80) != 0 && ((A^val) & 0x80) != 0;
			A = val & 0xff;
			cycles += 4;
			break;
		}
		case 0xfe: //INC increment memory, absoluteX
		{
			int abosluteXAddr = absoluteX();
			byte val = readAbsoluteX() + 1;
			Z = val == 0;
			N = (val & 0x80) == 0x80;
			memory->write(abosluteXAddr, val);
			cycles += 7;
			break;
		}

		default:
		{
			NOP();
			break;
		}
	}
}

void CPU6502::NMI()
{
	//push the PC to the stack
	push(PC);
	pushStatus(0);
	//set PC to the interrupt vector
	PC = memory->read(0xFFFA) | (memory->read(0xFFFA + 1) << 8);
	cycles += 7;
	IntDisable = 1; //disable interrupts while we are in NMI
}

void CPU6502::push(byte high, byte low)
{
	memory->write((SP) | 0x100, high);
	memory->write((SP - 1) | 0x100, low);
	SP -= 2;
}

void CPU6502::pushByte(byte val)
{
	memory->write((SP) | 0x100, val);
	SP--;
}

void CPU6502::push(int word)
{
	memory->write((SP) | 0x100, (word & 0xff00) >> 8);
	memory->write((SP - 1) | 0x100, word & 0xff);
	SP -= 2;
}

void CPU6502::pushStatus(bool bit4)
{
	//look at nesdev CPU_status_flag_behavior
	//TODO: this might not be correct
	byte statusByte = C 
		| (Z << 1)
		| (IntDisable << 2)
		| (D << 3)
		| (bit4 << 4)
		| (1 << 5)
		| (V << 6)
		| (N << 7);
	memory->write((SP) | 0x100, statusByte);
	SP--;
}

void CPU6502::pullStatus()
{
	//TODO: this might not be correct
	byte sByte = memory->read((SP + 1) | 0x100);
	SP++;
	Z = sByte & 0x2;
	C = sByte & 0x1;
IntDisable = sByte & 0x4;
D = sByte & 0x8;
V = sByte & 0x40;
N = sByte & 0x80;
}

int CPU6502::pullWord()
{
	int val = 0;
	val = memory->read((SP + 1) | 0x100) | (memory->read((SP + 2) | 0x100) << 8);
	SP += 2;
	return val & 0xffff;
}

byte CPU6502::pullByte()
{
	byte val = memory->read((SP + 1) | 0x100);
	SP++;
	PC++;
	return val & 0xff;
}

void CPU6502::NOP()
{
	//only increments the PC
	PC++;
}

void CPU6502::unimplementedOP()
{
	std::cout << "Unimplemented OP: 0x" << std::hex << (int)memory->read(PC) << std::endl;
	HALT = true;
}


unsigned char CPU6502::readImmediate()
{
	unsigned char val = memory->read(PC + 1);
	PC += 2;
	return val;
}

unsigned char CPU6502::readZeroPage()
{
	unsigned char addr = memory->read(PC + 1);
	unsigned char val = memory->read(addr & 0xff);
	PC += 2;
	return val;
}

unsigned char CPU6502::readZeroPageX()
{
	unsigned char addr = memory->read(PC + 1);
	unsigned char val = memory->read((addr + X) & 0xff);
	PC += 2;
	return val;
}

unsigned char CPU6502::readZeroPageY()
{
	unsigned char addr = memory->read(PC + 1);
	unsigned char val = memory->read((addr + Y) & 0xff);
	PC += 2;
	return val;
}

unsigned char CPU6502::readRelative()
{
	unsigned char val = memory->read(PC + 1);
	PC += 2;
	return val;
}

unsigned char CPU6502::readAbsolute()
{
	int addr = memory->read(PC + 2) << 8 | memory->read(PC + 1);
	unsigned char val = memory->read(addr) & 0xff;
	PC += 3;
	return val;
}

unsigned char CPU6502::readAbsoluteX()
{
	int addr = memory->read(PC + 2) << 8 | memory->read(PC + 1);
	unsigned char val = memory->read((addr + X)&0xffff) & 0xff;
	PC += 3;
	return val;
}

unsigned char CPU6502::readAbsoluteY()
{
	int addr = memory->read(PC + 2) << 8 | memory->read(PC + 1);
	unsigned char val = memory->read((addr + Y)&0xffff) & 0xff;
	PC += 3;
	return val;
}

int CPU6502::readIndirect()
{
	int addr = memory->read(PC + 2) << 8 | memory->read(PC + 1);
	int val;
	if ((addr & 0xff) == 0xff) {
		val = memory->read(addr&0xff00) << 8 | memory->read(addr);
	}
	else {
		val = memory->read(addr + 1) << 8 | memory->read(addr);
	}
	PC += 3;
	return val;
}

unsigned char CPU6502::readIndirectX()
{
	int addr1 = (memory->read(PC + 1) + X) & 0xff;
	int addr2 = memory->read((addr1 + 1)&0xff) << 8 | memory->read(addr1);
	unsigned char val = memory->read(addr2);
	PC += 2;
	return val;
}

unsigned char CPU6502::readIndirectY()
{
	int addr1 = memory->read(PC + 1);
	int addr2 = memory->read(addr1) | memory->read((addr1 + 1) & 0xff) << 8;
	addr2 += Y;
	addr2 &= 0xFFFF;
	unsigned char val = memory->read(addr2);
	PC += 2;
	return val;
}

byte CPU6502::zeroPage()
{
	return memory->read(PC + 1) & 0xff;
}

byte CPU6502::zeroPageX()
{
	return (memory->read(PC + 1) + X) & 0xff;
}

byte CPU6502::zeroPageY()
{
	return (memory->read(PC + 1) + Y) & 0xff;
}

int CPU6502::absolute()
{
	return (memory->read(PC + 2) << 8 | memory->read(PC + 1)) & 0xffff;
}

int CPU6502::absoluteX()
{
	return ((memory->read(PC + 2) << 8 | memory->read(PC + 1)) + X) & 0xffff;
}

int CPU6502::absoluteY()
{
	//this used to be:
	//return ((memory->read(PC + 2) << 8 | memory->read(PC + 1)) + X) & 0xffff;
	//but that doesnt seem right,								   /\ this X should be a Y
	//because absolute with offsets (X or Y) is supposed to offset by the value in the corresponding
	//register. Unless this is to account for some hardware bug that i have now forgotten
	// -06.28.18
	return ((memory->read(PC + 2) << 8 | memory->read(PC + 1)) + Y) & 0xffff;
}

int CPU6502::indirectX()
{
	int addr1 = (memory->read(PC + 1) + X) & 0xff;
	return  memory->read((addr1 + 1) & 0xff) << 8 | memory->read(addr1);
}

int CPU6502::indirectY()
{
	int addr1 = memory->read(PC + 1);
	int addr2 = memory->read(addr1) | memory->read((addr1 + 1) & 0xff) << 8;
	addr2 += Y;
	addr2 &= 0xFFFF;
	return addr2;
}

int CPU6502::readWord()
{
	int addr = 0;
	addr = memory->read(PC + 1) | (memory->read(PC + 2) << 8);
	PC += 3;
	return addr & 0xffff;
}

byte CPU6502::readByte()
{
	byte val = memory->read(PC + 1);
	PC += 2;
	return val;
}
