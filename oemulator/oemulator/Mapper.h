#pragma once

typedef unsigned char byte;
const int s2KB = 0x800;
const int s4KB = 0x1000;
const int s8KB = 0x2000;
const int s16KB = 0x4000;
const int s32KB = 0x8000;

class Mapper {
public:
	virtual byte read(int addr) = 0;
	virtual void write(int addr, byte val) = 0;
};

//TODO: add support for NES 2.0 and rename this to just header
struct iNESHeader {
	//the entire header
	byte fullHeader[16];
	//number of 16384 byte units of PRG ROM
	int numPRGROMUnits;
	//number of 8192 byte units of CHR ROM
	int numCHRROMUnits;
	bool verticalMirroring;
	//Does cart contain battery-backed PRG RAM ($6000-7FFF) 
	//or other persistent memory
	bool hasBatteryBackedPRGRAM;
	bool hasTrainer; //dont support this?
	//should the mirroring bit be ignored, if so use 4-screen VRAM
	bool ignoreMirroring;
};