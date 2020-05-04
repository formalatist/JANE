#pragma once
#include <iostream>

typedef unsigned char byte;
const int s2KB = 0x800;
const int s4KB = 0x1000;
const int s8KB = 0x2000;
const int s16KB = 0x4000;
const int s32KB = 0x8000;
const int s64KB = 0x10000;
const int s128KB = 0x20000;
const int s256KB = 0x40000;
const int s512KB = 0x80000;

enum MirrorMode {
	Horizontal,
	Vertical,
	Single0,
	Single1,
	Four
};

class Mapper {
public:
	virtual byte read(int addr) = 0;
	virtual void write(int addr, byte val) = 0;
	virtual void onExit() {
		std::cout << "Mapper hardware shutting down" << std::endl;
	};
	MirrorMode mirrorMode;
	iNESHeader header;

protected:
	void saveToFile(byte data[], int dataSize, std::string filename) {
		FILE* file = fopen(filename.c_str(), "wb");
		fwrite(data, 1, dataSize, file);
		fclose(file);
	}
};

//TODO: add support for NES 2.0 and rename this to just header
struct iNESHeader {
	//the entire header
	//byte fullHeader[16];
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

	iNESHeader(int nPRGROM, int nCHRROM,
		bool vM, bool bb, bool t, bool iM) : numPRGROMUnits(nPRGROM),
		numCHRROMUnits(nCHRROM), verticalMirroring(vM), hasBatteryBackedPRGRAM(bb),
		hasTrainer(t), ignoreMirroring(iM) {}
};