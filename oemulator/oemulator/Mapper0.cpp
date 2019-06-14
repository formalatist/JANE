#include "Mapper0.h"

Mapper0::Mapper0(byte header[], byte rom[])
{
	//setup PRG ROM
	int PRGROMSize = header[4]; //size in 16kb units
	std::cout << "PRG ROM SIZE: " << PRGROMSize << " * 16kb" << std::endl;
	for (int i = 0; i < 0x3fff; i++) {
		PRGROM1[i] = rom[i];
	}
	if(PRGROMSize > 1) { //we have 32kb rom
		for (int i = 0; i < 0x3fff; i++) {
			PRGROM2[i] = rom[i + 0x3fff];
		}
	} else { //only 16kb rom, mirror it in PRGROM2
		for (int i = 0; i < 0x3fff; i++) {
			PRGROM2[i] = rom[i];
		}
	}
}

void Mapper0::write(int addr, byte val)
{
	//PRGRAM is located at 0x6000-0x7fff
	PRGRAM[addr - 0x6000] = val;
}

byte Mapper0::read(int addr)
{
	if(addr <= 0x7fff) { //PRGRAM
		return PRGRAM[addr - 0x6000];
	}
	else if (addr <= 0xbfff) {
		return PRGROM1[addr - 0x8000];
	}
	else {
		return PRGROM2[addr - 0xc000];
	}
}
