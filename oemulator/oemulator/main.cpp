#define _CRT_SECURE_NO_DEPRECATE
#include <SDL.h>
#include <stdio.h>
#include <iostream>
#include "ROMLoader.h"
#include "NES.h"

bool running = true;

long getFileSize(FILE *file)
{
	long lCurPos, lEndPos;
	lCurPos = ftell(file);
	fseek(file, 0, 2);
	lEndPos = ftell(file);
	fseek(file, lCurPos, 0);
	return lEndPos;
}


int main(int argc, char** argv) {
	const char* filePath = "C:\\Users\\Oivind\\Documents\\GitHub\\oemulator\\roms\\nestest.nes";

	unsigned char* fileBuffer;
	FILE* file = NULL;

	if ((file = fopen(filePath, "rb")) == NULL) {
		std::cout << "Could not open file at path: " << filePath << std::endl;
		std::cin.get();
	}
	long fileSize = getFileSize(file);
	fileBuffer = new unsigned char[fileSize];
	fread(fileBuffer, fileSize, 1, file);
	fclose(file);

	//parse the iNES header
	std::cout << fileBuffer[0] << fileBuffer[1] << fileBuffer[2] << std::endl;
	std::cout << "Size of PRG ROM: " << static_cast<unsigned>(fileBuffer[4])
		<< "*16KB = " << static_cast<unsigned>(fileBuffer[4]) * 16 << "KB" << std::endl;
	std::cout << "Size of CHR ROM: " << static_cast<unsigned>(fileBuffer[5])
		<< "*8KB = " << static_cast<unsigned>(fileBuffer[5]) * 8 << "KB" << std::endl;
	std::cout << "Size of PRG RAM: " << static_cast<unsigned>(fileBuffer[8])
		<< "*8KB = " << static_cast<unsigned>(fileBuffer[8]) * 8 << "KB" << std::endl;



	std::cout <<( static_cast<int>(fileBuffer[7]) & 0x4) << 
		(static_cast<int>(fileBuffer[7]) & 0x8) << std::endl;

	//create the cpu
	//CPU6502 cpu = CPU6502();
	NES nes = NES();
	ROMLoader loader = ROMLoader(nes.memory);
	loader.loadROM(fileBuffer, fileSize, (*nes.cpu));
	//clear ppu registers
	loader.clearPPUReg();
	std::cout << "Starting PC at: 0x" << std::hex << nes.cpu->PC << std::endl;
	for (int i = 0; i < 50; i++)
	{	
		nes.cpu->step();
	}
	std::cout << "Total unique ops: " << std::dec << nes.cpu->numImplementedOps << std::endl;
	std::cout << "Done!" << std::endl;
	std::cout << "PC: " << std::hex << nes.cpu->PC << std::endl;

	std::cin.get();
	delete[] fileBuffer;
	return 0;
}