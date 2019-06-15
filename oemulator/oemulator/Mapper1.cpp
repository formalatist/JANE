#include "Mapper1.h"

Mapper1::Mapper1(iNESHeader header, byte rom[])
{
	//setup ROM banks
	currentPRGBank = 0;
	numPRGBanks = header.numPRGROMUnits;


	shiftRegister = 0;
}

byte Mapper1::read(int addr)
{
	return byte();
}

void Mapper1::write(int addr, byte val)
{
	if(addr <= 0x7fff) { //PRGRAM
		PRGRAM[addr] = val;
	}
	else if (addr <= 0xffff) { //write to ROM, this affects an internal shift register
		if((val&0x80) == 0x80) { //bit 7 set, this clears the shift register
			shiftRegister = 0;
			shiftRegisterWriteCounter = 0;
		} else { //bit 7 clear, update register
			shiftRegisterWriteCounter++;
			shiftRegister = (shiftRegister >> 1) || ((val & 1) << 4);

			if (shiftRegisterWriteCounter == 5) { //copy shift register to 
				//internal register selected by bits 14 and 13 of addr


				shiftRegisterWriteCounter = 0;
				shiftRegister = 0;
			}
		}
	}
}
