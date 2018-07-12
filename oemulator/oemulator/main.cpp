#define _CRT_SECURE_NO_DEPRECATE
#include <SDL.h>
#include <stdio.h>
#include <iostream>
#include "ROMLoader.h"
#include <chrono>
#include <ctime>
//#include "NES.h"

bool running = true;
const int FPS = 30;
int tickCounter = 0;


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
	const char* filePath = "C:\\Users\\Oivind\\Documents\\GitHub\\oemulator\\roms\\Donkey Kong (JU).nes";

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
	//loader.clearPPUReg();
	std::cout << "printing memory" << nes.memory->memory[0] << std::endl;
	std::cout << "Starting PC at: 0x" << std::hex << nes.cpu->PC << std::endl;
	

	//create the screen
	
	SDL_Window* mainWindow = NULL;
	SDL_Surface* mainWindowSurface = NULL;
	mainWindow = SDL_CreateWindow("ØMULATOR", 1024, 512,
		512, 240,
		SDL_WINDOW_SHOWN);
	mainWindowSurface = SDL_GetWindowSurface(mainWindow);
	nes.setScreen(mainWindowSurface, mainWindow);
	nes.updateScreen();
	

	//run the nes
	//nes.step(25000 * 120*2);
	//nes.updateScreen();

	
	//nes.cpu->printCallLog = true;
	//nes.step(9200); 


	std::cout << "Total unique ops: " << std::dec << nes.cpu->numImplementedOps << std::endl;
	std::cout << "Done!" << std::endl;
	std::cout << "PC: " << std::hex << nes.cpu->PC << std::endl;


	

	SDL_Window* patternTableWindow = NULL;
	SDL_Surface* patternTableSurface = NULL;

	patternTableWindow = SDL_CreateWindow("PatternTable", 0, 0,
		1024, 512,
		SDL_WINDOW_SHOWN);

	patternTableSurface = SDL_GetWindowSurface(patternTableWindow);
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
					Uint8 *targetPixel = (Uint8*)patternTableSurface->pixels + (y*4 + y1)*patternTableSurface->pitch
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
					Uint8 *targetPixel = (Uint8*)patternTableSurface->pixels + (y * 4 + y1)*patternTableSurface->pitch
						+ (x * 4 + x1) * 4;
					*(Uint32 *)targetPixel = color;
				}
			}
		}
	}

	SDL_UpdateWindowSurface(patternTableWindow);
	tickCounter = SDL_GetTicks();
	int frame = 0;
	int frameTime = 0;
	bool run = true;
	int start = 0;
	int start2 = 0;
	while (run) {
		start = clock();
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				run = false;
			}
		}
		start2 = clock();
		nes.stepSeconds(0.016f);
		std::cout << "Nes took: " << (clock() - start2) / double(CLOCKS_PER_SEC) * 1000 << std::endl;
		frameTime = SDL_GetTicks() - tickCounter;
		if (frameTime < 160) {
			SDL_Delay(160 - frameTime);
			std::cout << "test";
		}
		frame++;
		std::cout << frame << std::endl;
		std::cout << "Frame took: " << (clock() - start) / double(CLOCKS_PER_SEC) * 1000 << std::endl;
	}

	
	SDL_DestroyWindow(patternTableWindow);
	SDL_DestroyWindow(mainWindow);
	SDL_Quit();
	std::cin.get();

	delete[] fileBuffer;
	return 0;
}