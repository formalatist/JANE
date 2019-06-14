#include "Mapper0.h"

Mapper0::Mapper0(byte header[], byte rom[])
{
	//setup PRG ROM
	int PRGROMSize = header[4]; //size in 16kb units
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
