#include "Mappers/Mapper1.h"

Mapper1::Mapper1(iNESHeader header, byte rom[])
{
	//setup PRG banks
	PRGBank1 = 0;
	PRGBank2 = header.numPRGROMUnits-1;
	numPRGBanks = header.numPRGROMUnits;
	for (int i = 0; i < numPRGBanks*s16KB; i++) {
		PRGROM[i] = rom[i];
	}

	//setup CHR banks
	CHRBank1 = 0;
	CHRBank2 = 0;
	numCHRBanks = header.numCHRROMUnits;
	for (int i = 0; i < numCHRBanks*s8KB; i++) {
		CHRROM[i] = rom[i + numPRGBanks*s16KB];
	}

	//set mirroring
	if (header.verticalMirroring) {
mirrorMode = Vertical;
	}
	else {
		mirrorMode = Horizontal;
	}

	std::cout << "numPRGBanks: " << header.numPRGROMUnits << std::endl;
	std::cout << "numCHRBanks: " << header.numCHRROMUnits << std::endl;
	PRGSwapMode = 1;
	PRGBankSize = 1;
	shiftRegister = 0;
	shiftRegisterWriteCounter = 0;
	controlReg = 0xc;
	CHRROMReg1 = 0;
	CHRROMReg2 = 0;
	PRGROMReg = 0;
	updateBanksAndMirror();
}

byte Mapper1::read(int addr)
{
	addr &= 0xffff;
	if (addr <= 0xfff) { // CHR bank 1
		return CHRROM[addr + CHRBank1*s4KB];
	}
	else if (addr <= 0x1fff) { // CHR bank 2
		return CHRROM[addr + CHRBank2*s4KB - 0x1000];
	}
	else if (addr <= 0x7fff) { //PRGRAM
		return PRGRAM[addr - 0x6000];
	}
	else if (addr <= 0xbfff) { //PRG bank 1
		return PRGROM[addr + PRGBank1*s16KB - 0x8000];
	}
	else { //PRG bank 2
		return PRGROM[addr + PRGBank2*s16KB - 0xc000];
	}
}

void Mapper1::write(int addr, byte val)
{
	addr &= 0xffff;
	if (addr <= 0xfff) { // CHR bank 1
		CHRROM[addr + CHRBank1*s4KB] = val;
	}
	else if (addr <= 0x1fff) { // CHR bank 2
		CHRROM[addr + CHRBank2*s4KB - 0x1000] = val;
	}
	else if (addr <= 0x7fff) { //PRGRAM
		PRGRAM[addr - 0x6000] = val;
	}
	else if (addr <= 0xffff) { //write to ROM, this affects an internal shift register
		if ((val & 0x80) == 0x80) { //bit 7 set, this clears the shift register
			shiftRegister = 0; //its unclear if this should be 0 or 0x10, it should not matter, but it might
			shiftRegisterWriteCounter = 0;
			controlReg |= 0xc;
			updateBanksAndMirror();

		}
		else { //bit 7 clear, update register
			shiftRegisterWriteCounter++;
			shiftRegister = (shiftRegister >> 1) | ((val & 1) << 4);

			if (shiftRegisterWriteCounter == 5) { //copy shift register to 
				//internal register selected by bits 14 and 13 of addr
				writeRegister(addr);

				shiftRegisterWriteCounter = 0;
				shiftRegister = 0;

				updateBanksAndMirror();
			}
		}
	}
}

void Mapper1::writeRegister(int addr)
{
	shiftRegister &= 0b11111;
	//std::cout << "WRITING TO REG" << std::endl;
	if (addr <= 0x9fff) { // reg 0, Control register
		controlReg = shiftRegister;
	}
	else if (addr <= 0xbfff) { // reg 1, CHR ROM bank register
		CHRROMReg1 = shiftRegister;
	}
	else if (addr <= 0xdfff) { // reg 2, CHR ROM bank register
		CHRROMReg2 = shiftRegister;
	}
	else if(addr <= 0xffff) { // reg 3, PRG ROM bank register
		PRGROMReg = shiftRegister&0xf;
	}
}

void Mapper1::updateBanksAndMirror()
{
	if ((controlReg & 0b1000) == 0b1000) { //16kb PRG banks
		std::cout << "16BK PRG BANKS" << std::endl;
		if ((controlReg & 0b100) == 0b100) { //Bank C000-FFFFh is fixed, while 8000-FFFFh is swappable.
			PRGBank1 = PRGROMReg & 0xf;
			PRGBank2 = numPRGBanks-1;
		}
		else { //Bank 8000-BFFFh is fixed, while C000-FFFFh is swappable
			PRGBank1 = 0;
			PRGBank2 = PRGROMReg & 0xf;
		}
	}
	else { //32kb PRG banks
		std::cout << "32BK PRG BANKS" << std::endl;
		PRGBank1 = (PRGROMReg & 0b1110) >> 1;
		PRGBank2 = PRGBank1 + 1;
	}

	if((controlReg & 0b10000) == 0b10000) { //4kb CHR banks
		CHRBank1 = CHRROMReg1;
		CHRBank2 = CHRROMReg2;
	} else { //8kb CHR banks
		CHRBank1 = (CHRROMReg1 >> 1);
		CHRBank2 = CHRBank1 + 1;
	}

	if ((controlReg & 0b11) == 2) {
		mirrorMode = MirrorMode::Vertical;
	}
	else if ((controlReg & 0b11) == 3) {
		mirrorMode = MirrorMode::Horizontal;
	}

	std::cout << "CHRBank1: " << CHRBank1 << " CHRBank2: " << CHRBank2 << std::endl;
	std::cout << "PRGBank1: " << PRGBank1 << " PRGBank2: " << PRGBank2 << std::endl;
}
