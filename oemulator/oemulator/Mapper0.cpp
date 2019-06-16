#include "Mapper0.h"

//TODO switch to suing iNESHeader
Mapper0::Mapper0(byte header[], byte rom[])
{
	//setup PRG ROM
	int PRGROMSize = header[4]; //size in 16kb units
	
	for (int i = 0; i < s16KB; i++) {
		PRGROM1[i] = rom[i];
	}
	if(PRGROMSize > 1) { //we have 32kb rom
		for (int i = 0; i < s16KB; i++) {
			PRGROM2[i] = rom[i + s16KB];
		}
	} else { //only 16kb rom, mirror it in PRGROM2
		for (int i = 0; i < 0x4000; i++) {
			PRGROM2[i] = rom[i];
		}
	}
	std::cout << "PRGroMSIXE: " << PRGROMSize << std::endl;
	//setup CHR ROM
	for (int i = 0; i < s8KB; i++) {
		CHRROM[i] = rom[i + PRGROMSize*s16KB];
	}
}

void Mapper0::write(int addr, byte val)
{
	//PRGRAM is located at 0x6000-0x7fff
	std::cout << "WRITE TO MAPPER: " << addr << "  val: " << val << std::endl;
	//PRGRAM[addr - 0x6000] = val;
}

byte Mapper0::read(int addr)
{
	if (addr <= 0x1fff) {
		return CHRROM[addr];
	}
	else if(addr <= 0x7fff) { //PRGRAM
		return PRGRAM[addr - 0x6000];
	}
	else if (addr <= 0xbfff) {
		return PRGROM1[addr - 0x8000];
	}
	else {
		return PRGROM2[addr - 0xc000];
	}
}
