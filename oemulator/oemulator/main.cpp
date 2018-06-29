#define _CRT_SECURE_NO_DEPRECATE
#include <SDL.h>
#include <stdio.h>
#include <iostream>
#include "ROMLoader.h"
//#include "NES.h"

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
	const char* filePath = "C:\\Users\\Oivind\\Documents\\GitHub\\oemulator\\roms\\donkey-kong.nes";

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
	ROMLoader loader = ROMLoader(nes.memory, nes.ppuMemory);
	loader.loadROM(fileBuffer, fileSize, (*nes.cpu));
	//clear ppu registers
	loader.clearPPUReg();
	std::cout << "printing memory" << nes.memory->memory[0] << std::endl;
	std::cout << "Starting PC at: 0x" << std::hex << nes.cpu->PC << std::endl;
	for (int i = 0; i < 50; i++)
	{	
		nes.cpu->step();
	}
	std::cout << "Total unique ops: " << std::dec << nes.cpu->numImplementedOps << std::endl;
	std::cout << "Done!" << std::endl;
	std::cout << "PC: " << std::hex << nes.cpu->PC << std::endl;

	SDL_Window* window = NULL;
	SDL_Surface* screenSurface = NULL;

	window = SDL_CreateWindow("�MULATOR", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		1024, 512,
		SDL_WINDOW_SHOWN);

	screenSurface = SDL_GetWindowSurface(window);
	int colors[4] = { 0x666666, 0xFFCCC5, 0x1412A7, 0xB53120 };
	for (int x = 0; x < 128; x++) {
		for (int y = 0; y < 128; y++) {
			int col = x / 8;
			int row = y / 8;
			int address1 = (y%8 ) | (1 << 3) | (col << 4 & 0b11110000) | (row << 8 & 0xF00) ;
			int address2 = (y % 8) | (col << 4 & 0b11110000) | (row << 8 & 0xF00);
			int xOffset = 7 - x % 8;
			int color = (nes.ppuMemory->memory[address1]>>xOffset & 1)
				+ 2*(nes.ppuMemory->memory[address2]>>xOffset & 1);
			if (color < 0 || color > 3) {
				std::cout << "ERROR: " << color << std::endl;
			}
			color = colors[color];
			for (int x1 = 0; x1 < 4; x1++) {
				for (int y1 = 0; y1 < 4; y1++) {
					Uint8 *targetPixel = (Uint8*)screenSurface->pixels + (y*4 + y1)*screenSurface->pitch 
						+ (x*4 + x1) * 4;
					*(Uint32 *)targetPixel = color;
				}
			}
		}
	}

    int colors2[4] = { 0, 0x48CDDE, 0xB71E7B, 0x4240FF };
	for (int x = 128; x < 256; x++) {
		for (int y = 0; y < 128; y++) {
			int col = x / 8;
			int row = y / 8;
			int address1 = (y % 8) | (1 << 3) | (col << 4 & 0b11110000) | (row << 8 & 0xF00) | 0x1000;
			int address2 = (y % 8) | (col << 4 & 0b11110000) | (row << 8 & 0xF00) | 0x1000;
			int xOffset = 7 - x % 8;
			int color = (nes.ppuMemory->memory[address1] >> xOffset & 1)
				+ 2 * (nes.ppuMemory->memory[address2] >> xOffset & 1);
			color = colors2[color];
			for (int x1 = 0; x1 < 4; x1++) {
				for (int y1 = 0; y1 < 4; y1++) {
					Uint8 *targetPixel = (Uint8*)screenSurface->pixels + (y * 4 + y1)*screenSurface->pitch
						+ (x * 4 + x1) * 4;
					*(Uint32 *)targetPixel = color;
				}
			}
		}
	}

	SDL_UpdateWindowSurface(window);

	bool run = true;
	while (run) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				run = false;
			}
		}
	}

	std::cin.get();
	SDL_DestroyWindow(window);
	SDL_Quit();

	delete[] fileBuffer;
	return 0;
}