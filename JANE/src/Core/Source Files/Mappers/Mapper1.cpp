#define _CRT_SECURE_NO_WARNINGS
#include "Mappers/Mapper1.h"

Mapper1::Mapper1(iNESHeader header, byte rom[], std::string ROMName_, std::string saveDir_)
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

	shiftRegister = 0;
	ROMName = ROMName_;
	saveDir = saveDir_;
	loadPRGRAM();
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
	} else if(addr <= 0xbfff) { //PRG bank 1
		return PRGROM[addr + PRGBank1*s16KB - 0x8000];
	} else { //PRG bank 2
		return PRGROM[addr + PRGBank2*s16KB - 0xc000];
	}
}

void Mapper1::write(int addr, byte val)
{
	addr &= 0xffff;
	if (addr <= 0xfff) { // CHR bank 1
		CHRROM[addr + CHRBank1*s4KB] = val;
	} 
	else if(addr <= 0x1fff) { // CHR bank 2
		CHRROM[addr + CHRBank2*s4KB - 0x1000] = val;
	}
	else if(addr <= 0x7fff) { //PRGRAM
		PRGRAM[addr - 0x6000] = val;
	}
	else if (addr <= 0xffff) { //write to ROM, this affects an internal shift register
		if((val&0x80) == 0x80) { //bit 7 set, this clears the shift register
			shiftRegister = 0;
			shiftRegisterWriteCounter = 0;
		} else { //bit 7 clear, update register
			shiftRegisterWriteCounter++;
			shiftRegister = (shiftRegister >> 1) | ((val & 1) << 4);

			if (shiftRegisterWriteCounter == 5) { //copy shift register to 
				//internal register selected by bits 14 and 13 of addr
				writeRegister(addr);

				shiftRegisterWriteCounter = 0;
				shiftRegister = 0;
			}
		}
	}
}



void Mapper1::writeRegister(int addr)
{
	//std::cout << "WRITING TO REG" << std::endl;
	if(addr <= 0x9fff) { // reg 0, Control register
		//hook mappers up in such a way that
		//they can control mirroring from here, then
		//add support for mirror types 3 and 4
		//std::cout << "WRITING TO CONTROL REG" << std::endl;
		//std::cout << "Mirror mode set to : " << (int)(shiftRegister & 0b11) << std::endl;
		byte mirror = shiftRegister & 0b11;
		if(mirror == 0) { //1-screen mirroring (nametable 0)
			mirrorMode = Single0;
		} else if(mirror == 1) { //1-screen mirroring (nametable 1)
			mirrorMode = Single1;
		} else if(mirror == 2) { //Vertical
			mirrorMode = Vertical;
		} else {
			mirrorMode = Horizontal;
		}

		PRGSwapMode = (shiftRegister & 0b100) >> 2;
		PRGBankSize = (shiftRegister & 0b1000) >> 3;
		CHRBankSize = (shiftRegister & 0b10000) >> 4;
	}
	else if (addr <= 0xbfff) { // reg 1, CHR ROM bank register
		std::cout << "WRITING TO REG 1" << std::endl;
		if (CHRBankSize == 0) {
			CHRBank1 = ((shiftRegister & 0b11110)>>1)*2;
			CHRBank2 = CHRBank1 + 1;
		}
		else {
			CHRBank1 = shiftRegister;
		}
	}
	else if (addr <= 0xdfff) { // reg 2, CHR ROM bank register
		std::cout << "WRITING TO REG 2" << std::endl;
		if (CHRBankSize == 0) {
			//CHRBank2 = shiftRegister;
			std::cout << "WRITE TO CHR ROM REG 2 THAT WONT COUNT###############" << std::endl;
		}
		else {
			CHRBank2 = shiftRegister;
		}
	}
	else { // reg 3, PRG ROM bank register
		//std::cout << "WRITING TO REG 3" << std::endl;
		if (PRGBankSize == 0) {
			PRGBank1 = ((shiftRegister & 0b1110) >> 1)*2;
			PRGBank2 = PRGBank1 + 1;
		}
		else {
			if(PRGSwapMode == 0) { // 0x8000-0xbfff fixed, 0xc000-0xffff swappable
				PRGBank1 = 0;
				PRGBank2 = shiftRegister & 0b1111;
			}
			else {
				PRGBank1 = shiftRegister & 0b1111;
				PRGBank2 = numPRGBanks-1;
			}
		}
		//std::cout << "PRGBank1 and 2 is now : " << PRGBank1 << "  " << PRGBank2 << std::endl;
	}
}

void Mapper1::onExit()
{
	savePRGRAM();
}

void Mapper1::savePRGRAM()
{
	FILE* file = NULL;
	if ((file = fopen((saveDir + ROMName).c_str(), "wb")) == NULL) {
		std::cout << "could not open file for saving: " << (saveDir + ROMName) << std::endl;
	}
	else {
		std::cout << "saving PRGRAM" << std::endl;
		fwrite(PRGRAM, 1, s8KB, file);
		fclose(file);
	}
}

void Mapper1::loadPRGRAM()
{
	FILE* file = NULL;
	if ((file = fopen((saveDir + ROMName).c_str(), "rb")) == NULL) {
		std::cout << "could not open save file: " << (saveDir + ROMName) << std::endl;
	}
	else {
		std::cout << "loading PRGRAM" << std::endl;
		fread(PRGRAM, 1, s8KB, file);
		fclose(file);
	}
}
