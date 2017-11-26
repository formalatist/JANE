#include "CPU6502.h"

CPU6502::CPU6502()
{
	//do nothign for now
}

void CPU6502::loadMemory(std::vector<unsigned char> rom, int offset)
{
	int memoryI = offset;
	for (std::vector<unsigned char>::iterator i = rom.begin(); i != rom.end(); ++i)
	{
		memory[memoryI] = *i;
		memoryI++;
	}
}

void CPU6502::loadMemory(unsigned char rom[], int romSize, int offset)
{
	for (int i = 0; i < romSize; i++)
	{
		memory[i + offset] = rom[i];
	}
}

void CPU6502::step()
{
	executeOP();
}

void CPU6502::executeOP()
{
	switch (memory[PC])
	{
	case 0x0:
		unimplementedOP();
		break;
	case 0x1:
		unimplementedOP();
		break;
	case 0x2:
		unimplementedOP();
		break;
	case 0x3:
		unimplementedOP();
		break;
	case 0x4:
		unimplementedOP();
		break;
	case 0x5:
		unimplementedOP();
		break;
	case 0x6:
		unimplementedOP();
		break;
	case 0x7:
		unimplementedOP();
		break;
	case 0x8:
		unimplementedOP();
		break;
	case 0x9:
		unimplementedOP();
		break;
	case 0xa:
		unimplementedOP();
		break;
	case 0xb:
		unimplementedOP();
		break;
	case 0xc:
		unimplementedOP();
		break;
	case 0xd:
		unimplementedOP();
		break;
	case 0xe:
		unimplementedOP();
		break;
	case 0xf:
		unimplementedOP();
		break;
	case 0x10:
		unimplementedOP();
		break;
	case 0x11:
		unimplementedOP();
		break;
	case 0x12:
		unimplementedOP();
		break;
	case 0x13:
		unimplementedOP();
		break;
	case 0x14:
		unimplementedOP();
		break;
	case 0x15:
		unimplementedOP();
		break;
	case 0x16:
		unimplementedOP();
		break;
	case 0x17:
		unimplementedOP();
		break;
	case 0x18:
		unimplementedOP();
		break;
	case 0x19:
		unimplementedOP();
		break;
	case 0x1a:
		unimplementedOP();
		break;
	case 0x1b:
		unimplementedOP();
		break;
	case 0x1c:
		unimplementedOP();
		break;
	case 0x1d:
		unimplementedOP();
		break;
	case 0x1e:
		unimplementedOP();
		break;
	case 0x1f:
		unimplementedOP();
		break;
	case 0x20:
		unimplementedOP();
		break;
	case 0x21:
		unimplementedOP();
		break;
	case 0x22:
		unimplementedOP();
		break;
	case 0x23:
		unimplementedOP();
		break;
	case 0x24:
		unimplementedOP();
		break;
	case 0x25:
		unimplementedOP();
		break;
	case 0x26:
		unimplementedOP();
		break;
	case 0x27:
		unimplementedOP();
		break;
	case 0x28:
		unimplementedOP();
		break;
	case 0x29:
		unimplementedOP();
		break;
	case 0x2a:
		unimplementedOP();
		break;
	case 0x2b:
		unimplementedOP();
		break;
	case 0x2c:
		unimplementedOP();
		break;
	case 0x2d:
		unimplementedOP();
		break;
	case 0x2e:
		unimplementedOP();
		break;
	case 0x2f:
		unimplementedOP();
		break;
	case 0x30:
		unimplementedOP();
		break;
	case 0x31:
		unimplementedOP();
		break;
	case 0x32:
		unimplementedOP();
		break;
	case 0x33:
		unimplementedOP();
		break;
	case 0x34:
		unimplementedOP();
		break;
	case 0x35:
		unimplementedOP();
		break;
	case 0x36:
		unimplementedOP();
		break;
	case 0x37:
		unimplementedOP();
		break;
	case 0x38:
		unimplementedOP();
		break;
	case 0x39:
		unimplementedOP();
		break;
	case 0x3a:
		unimplementedOP();
		break;
	case 0x3b:
		unimplementedOP();
		break;
	case 0x3c:
		unimplementedOP();
		break;
	case 0x3d:
		unimplementedOP();
		break;
	case 0x3e:
		unimplementedOP();
		break;
	case 0x3f:
		unimplementedOP();
		break;
	case 0x40:
		unimplementedOP();
		break;
	case 0x41:
		unimplementedOP();
		break;
	case 0x42:
		unimplementedOP();
		break;
	case 0x43:
		unimplementedOP();
		break;
	case 0x44:
		unimplementedOP();
		break;
	case 0x45:
		unimplementedOP();
		break;
	case 0x46:
		unimplementedOP();
		break;
	case 0x47:
		unimplementedOP();
		break;
	case 0x48:
		unimplementedOP();
		break;
	case 0x49:
		unimplementedOP();
		break;
	case 0x4a:
		unimplementedOP();
		break;
	case 0x4b:
		unimplementedOP();
		break;
	case 0x4c:
		unimplementedOP();
		break;
	case 0x4d:
		unimplementedOP();
		break;
	case 0x4e:
		unimplementedOP();
		break;
	case 0x4f:
		unimplementedOP();
		break;
	case 0x50:
		unimplementedOP();
		break;
	case 0x51:
		unimplementedOP();
		break;
	case 0x52:
		unimplementedOP();
		break;
	case 0x53:
		unimplementedOP();
		break;
	case 0x54:
		unimplementedOP();
		break;
	case 0x55:
		unimplementedOP();
		break;
	case 0x56:
		unimplementedOP();
		break;
	case 0x57:
		unimplementedOP();
		break;
	case 0x58:
		unimplementedOP();
		break;
	case 0x59:
		unimplementedOP();
		break;
	case 0x5a:
		unimplementedOP();
		break;
	case 0x5b:
		unimplementedOP();
		break;
	case 0x5c:
		unimplementedOP();
		break;
	case 0x5d:
		unimplementedOP();
		break;
	case 0x5e:
		unimplementedOP();
		break;
	case 0x5f:
		unimplementedOP();
		break;
	case 0x60:
		unimplementedOP();
		break;
	case 0x61:
		unimplementedOP();
		break;
	case 0x62:
		unimplementedOP();
		break;
	case 0x63:
		unimplementedOP();
		break;
	case 0x64:
		unimplementedOP();
		break;
	case 0x65:
		unimplementedOP();
		break;
	case 0x66:
		unimplementedOP();
		break;
	case 0x67:
		unimplementedOP();
		break;
	case 0x68:
		unimplementedOP();
		break;
	case 0x69:
		unimplementedOP();
		break;
	case 0x6a:
		unimplementedOP();
		break;
	case 0x6b:
		unimplementedOP();
		break;
	case 0x6c:
		unimplementedOP();
		break;
	case 0x6d:
		unimplementedOP();
		break;
	case 0x6e:
		unimplementedOP();
		break;
	case 0x6f:
		unimplementedOP();
		break;
	case 0x70:
		unimplementedOP();
		break;
	case 0x71:
		unimplementedOP();
		break;
	case 0x72:
		unimplementedOP();
		break;
	case 0x73:
		unimplementedOP();
		break;
	case 0x74:
		unimplementedOP();
		break;
	case 0x75:
		unimplementedOP();
		break;
	case 0x76:
		unimplementedOP();
		break;
	case 0x77:
		unimplementedOP();
		break;
	case 0x78:
		unimplementedOP();
		break;
	case 0x79:
		unimplementedOP();
		break;
	case 0x7a:
		unimplementedOP();
		break;
	case 0x7b:
		unimplementedOP();
		break;
	case 0x7c:
		unimplementedOP();
		break;
	case 0x7d:
		unimplementedOP();
		break;
	case 0x7e:
		unimplementedOP();
		break;
	case 0x7f:
		unimplementedOP();
		break;
	case 0x80:
		unimplementedOP();
		break;
	case 0x81:
		unimplementedOP();
		break;
	case 0x82:
		unimplementedOP();
		break;
	case 0x83:
		unimplementedOP();
		break;
	case 0x84:
		unimplementedOP();
		break;
	case 0x85:
		unimplementedOP();
		break;
	case 0x86:
		unimplementedOP();
		break;
	case 0x87:
		unimplementedOP();
		break;
	case 0x88:
		unimplementedOP();
		break;
	case 0x89:
		unimplementedOP();
		break;
	case 0x8a:
		unimplementedOP();
		break;
	case 0x8b:
		unimplementedOP();
		break;
	case 0x8c:
		unimplementedOP();
		break;
	case 0x8d:
		unimplementedOP();
		break;
	case 0x8e:
		unimplementedOP();
		break;
	case 0x8f:
		unimplementedOP();
		break;
	case 0x90:
		unimplementedOP();
		break;
	case 0x91:
		unimplementedOP();
		break;
	case 0x92:
		unimplementedOP();
		break;
	case 0x93:
		unimplementedOP();
		break;
	case 0x94:
		unimplementedOP();
		break;
	case 0x95:
		unimplementedOP();
		break;
	case 0x96:
		unimplementedOP();
		break;
	case 0x97:
		unimplementedOP();
		break;
	case 0x98:
		unimplementedOP();
		break;
	case 0x99:
		unimplementedOP();
		break;
	case 0x9a:
		unimplementedOP();
		break;
	case 0x9b:
		unimplementedOP();
		break;
	case 0x9c:
		unimplementedOP();
		break;
	case 0x9d:
		unimplementedOP();
		break;
	case 0x9e:
		unimplementedOP();
		break;
	case 0x9f:
		unimplementedOP();
		break;
	case 0xa0:
		unimplementedOP();
		break;
	case 0xa1:
		unimplementedOP();
		break;
	case 0xa2:
		unimplementedOP();
		break;
	case 0xa3:
		unimplementedOP();
		break;
	case 0xa4:
		unimplementedOP();
		break;
	case 0xa5:
		unimplementedOP();
		break;
	case 0xa6:
		unimplementedOP();
		break;
	case 0xa7:
		unimplementedOP();
		break;
	case 0xa8:
		unimplementedOP();
		break;
	case 0xa9:
		unimplementedOP();
		break;
	case 0xaa:
		unimplementedOP();
		break;
	case 0xab:
		unimplementedOP();
		break;
	case 0xac:
		unimplementedOP();
		break;
	case 0xad:
		unimplementedOP();
		break;
	case 0xae:
		unimplementedOP();
		break;
	case 0xaf:
		unimplementedOP();
		break;
	case 0xb0:
		unimplementedOP();
		break;
	case 0xb1:
		unimplementedOP();
		break;
	case 0xb2:
		unimplementedOP();
		break;
	case 0xb3:
		unimplementedOP();
		break;
	case 0xb4:
		unimplementedOP();
		break;
	case 0xb5:
		unimplementedOP();
		break;
	case 0xb6:
		unimplementedOP();
		break;
	case 0xb7:
		unimplementedOP();
		break;
	case 0xb8:
		unimplementedOP();
		break;
	case 0xb9:
		unimplementedOP();
		break;
	case 0xba:
		unimplementedOP();
		break;
	case 0xbb:
		unimplementedOP();
		break;
	case 0xbc:
		unimplementedOP();
		break;
	case 0xbd:
		unimplementedOP();
		break;
	case 0xbe:
		unimplementedOP();
		break;
	case 0xbf:
		unimplementedOP();
		break;
	case 0xc0:
		unimplementedOP();
		break;
	case 0xc1:
		unimplementedOP();
		break;
	case 0xc2:
		unimplementedOP();
		break;
	case 0xc3:
		unimplementedOP();
		break;
	case 0xc4:
		unimplementedOP();
		break;
	case 0xc5:
		unimplementedOP();
		break;
	case 0xc6:
		unimplementedOP();
		break;
	case 0xc7:
		unimplementedOP();
		break;
	case 0xc8:
		unimplementedOP();
		break;
	case 0xc9:
		unimplementedOP();
		break;
	case 0xca:
		unimplementedOP();
		break;
	case 0xcb:
		unimplementedOP();
		break;
	case 0xcc:
		unimplementedOP();
		break;
	case 0xcd:
		unimplementedOP();
		break;
	case 0xce:
		unimplementedOP();
		break;
	case 0xcf:
		unimplementedOP();
		break;
	case 0xd0:
		unimplementedOP();
		break;
	case 0xd1:
		unimplementedOP();
		break;
	case 0xd2:
		unimplementedOP();
		break;
	case 0xd3:
		unimplementedOP();
		break;
	case 0xd4:
		unimplementedOP();
		break;
	case 0xd5:
		unimplementedOP();
		break;
	case 0xd6:
		unimplementedOP();
		break;
	case 0xd7:
		unimplementedOP();
		break;
	case 0xd8:
		unimplementedOP();
		break;
	case 0xd9:
		unimplementedOP();
		break;
	case 0xda:
		unimplementedOP();
		break;
	case 0xdb:
		unimplementedOP();
		break;
	case 0xdc:
		unimplementedOP();
		break;
	case 0xdd:
		unimplementedOP();
		break;
	case 0xde:
		unimplementedOP();
		break;
	case 0xdf:
		unimplementedOP();
		break;
	case 0xe0:
		unimplementedOP();
		break;
	case 0xe1:
		unimplementedOP();
		break;
	case 0xe2:
		unimplementedOP();
		break;
	case 0xe3:
		unimplementedOP();
		break;
	case 0xe4:
		unimplementedOP();
		break;
	case 0xe5:
		unimplementedOP();
		break;
	case 0xe6:
		unimplementedOP();
		break;
	case 0xe7:
		unimplementedOP();
		break;
	case 0xe8:
		unimplementedOP();
		break;
	case 0xe9:
		unimplementedOP();
		break;
	case 0xea:
		unimplementedOP();
		break;
	case 0xeb:
		unimplementedOP();
		break;
	case 0xec:
		unimplementedOP();
		break;
	case 0xed:
		unimplementedOP();
		break;
	case 0xee:
		unimplementedOP();
		break;
	case 0xef:
		unimplementedOP();
		break;
	case 0xf0:
		unimplementedOP();
		break;
	case 0xf1:
		unimplementedOP();
		break;
	case 0xf2:
		unimplementedOP();
		break;
	case 0xf3:
		unimplementedOP();
		break;
	case 0xf4:
		unimplementedOP();
		break;
	case 0xf5:
		unimplementedOP();
		break;
	case 0xf6:
		unimplementedOP();
		break;
	case 0xf7:
		unimplementedOP();
		break;
	case 0xf8:
		unimplementedOP();
		break;
	case 0xf9:
		unimplementedOP();
		break;
	case 0xfa:
		unimplementedOP();
		break;
	case 0xfb:
		unimplementedOP();
		break;
	case 0xfc:
		unimplementedOP();
		break;
	case 0xfd:
		unimplementedOP();
		break;
	case 0xfe:
		unimplementedOP();
		break;
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

void CPU6502::unimplementedOP()
{
	std::cout << "Unimplemented OP: 0x" << std::hex << memory[PC] << std::endl;
}

unsigned char CPU6502::readImmediate()
{
	unsigned char val = memory[PC];
	PC += 2;
	return val;
}

unsigned char CPU6502::readZeroPage()
{
	unsigned char addr = memory[PC + 1];
	unsigned char val = memory[addr & 0xff];
	PC += 2;
	return val;
}

unsigned char CPU6502::readZeroPageX()
{
	unsigned char addr = memory[PC + 1];
	unsigned char val = memory[(addr + X) & 0xff];
	PC += 2;
	return val;
}

unsigned char CPU6502::readZeroPageY()
{
	unsigned char addr = memory[PC + 1];
	unsigned char val = memory[(addr + Y) & 0xff];
	PC += 2;
	return val;
}

unsigned char CPU6502::readRelative()
{
	unsigned char val = memory[PC + 1];
	PC += 2;
	//check if val is positive or negative
	//TODO this is probably not correct since val is propably in twoscompliment?
	return val & 0x80 ? -(val & 0x7f) : val & 0x7f;
}

unsigned char CPU6502::readAbsolute() 
{
	int addr = memory[PC + 2] << 8 | memory[PC + 1];
	unsigned char val = memory[addr] & 0xff;
	PC += 3;
	return val;
}

unsigned char CPU6502::readAbsoluteX()
{
	int addr = memory[PC + 2] << 8 | memory[PC + 1];
	unsigned char val = memory[addr + X] & 0xff;
	PC += 3;
	return val;
}

unsigned char CPU6502::readAbsoluteY()
{
	int addr = memory[PC + 2] << 8 | memory[PC + 1];
	unsigned char val = memory[addr + Y] & 0xff;
	PC += 3;
	return val;
}

unsigned char CPU6502::readIndirect()
{
	int addr = memory[PC + 2] << 8 | memory[PC + 1];
	int val = memory[addr + 1] << 8 | memory[addr];
	PC += 3;
	return val;
}

unsigned char CPU6502::readIndirectX()
{
	int addr1 = (memory[PC + 1] + X) & 0xff;
	int addr2 = memory[addr1 + 1] << 8 | memory[addr1];
	//TODO: addr2 + 1 might have to wrap around and it might not.
	unsigned char val = memory[addr2 + 1] << 8 | memory[addr2];
	PC += 2;
	return val;
}

unsigned char CPU6502::readIndirectY()
{
	int addr1 = memory[PC + 1] << 8 | memory[PC] + Y;
	int addr2 = memory[addr1 + 1] << 8 | memory[addr1];
	unsigned char val = memory[addr2];
	PC += 2;
	return val;
}
