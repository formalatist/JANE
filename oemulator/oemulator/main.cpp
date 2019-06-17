#define _CRT_SECURE_NO_DEPRECATE
#include <SDL.h>
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include "ROMLoader.h"
#include <chrono>
#include <ctime>
#include <string>

bool running = true;
const int FPS = 30;
int tickCounter = 0;
const int SCALE = 2;


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
  
	std::string game = "cpu_timing_test";
	std::string filePath = "C:\\Users\\Oivind\\Documents\\GitHub\\oemulator\\roms\\" + game + ".nes";


	unsigned char* fileBuffer;
	FILE* file = NULL;

	if ((file = fopen(filePath.c_str(), "rb")) == NULL) {
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
	NES nes = NES();
	Controller c1 = Controller();
	nes.connectController(&c1);
	ROMLoader loader = ROMLoader(nes.memory, nes.ppuMemory);
	loader.loadROM(fileBuffer, fileSize, (*nes.cpu));

	std::cout << "Starting PC at: 0x" << std::hex << nes.cpu->PC << std::endl;
	

	//create the screen
	SDL_Window* mainWindow = NULL;
	SDL_Surface* mainWindowSurface = NULL;
	SDL_Surface* scaledMainWindowSurface = NULL;
	mainWindow = SDL_CreateWindow("ØMULATOR", 512, 256,
		512*SCALE, 240*SCALE,
		SDL_WINDOW_SHOWN);
	mainWindowSurface = SDL_GetWindowSurface(mainWindow);
	/*Uint32 rmask = 0xff000000;
	Uint32 gmask = 0x00ff0000;
	Uint32 bmask = 0x0000ff00;
	Uint32 amask = 0x000000ff;
	mainWindowSurface = SDL_CreateRGBSurface(0,512, 240,
		32, rmask, gmask, bmask, amask);*/
	nes.setScreen(mainWindowSurface, scaledMainWindowSurface, mainWindow);
	nes.updateScreen();

	
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
			int color = (nes.ppuMemory->read(address1)>>xOffset & 1)
				+ 2*(nes.ppuMemory->read(address2)>>xOffset & 1);
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
			int color = (nes.ppuMemory->read(address1) >> xOffset & 1)
				+ 2 * (nes.ppuMemory->read(address2) >> xOffset & 1);
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
	SDL_Event event;
	byte input = 0;

	//nes.step(1000000);
	//nes.cpu->printCallLog = true;
	//nes.step(20);
	//run = false;
	char* dir;
	while (run) {
		start = clock();
		c1.releaseButton(0xFF);
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				run = false;
			}
			else if (event.type == SDL_KEYDOWN) {
				std::cout << "KEYDOWN############" << event.key.keysym.sym << std::endl;
				switch (event.key.keysym.sym) {
				case SDLK_a:		
					input |= 0x1;
					std::cout << 1 << " JUMP (A)" << std::endl;
					break;
				case SDLK_b:
					input |= 0x2;
					std::cout << 2 << std::endl;
					break;
				case SDLK_s:
					input |= 0x4;
					std::cout << 3 << " DOWN" << std::endl;
					break;
				case SDLK_SPACE:
					input |= 0x8;
					std::cout << 4 << " START" << std::endl;
					break;
				case SDLK_UP:
					input |= 0x10;
					std::cout << 5 << " UP" << std::endl;
					break;
				case SDLK_DOWN:
					input |= 0x20;
					std::cout << 6 << " DOWN" << std::endl;
					break;
				case SDLK_LEFT:
					input |= 0x40;
					std::cout << 7 << " LEFT" << std::endl;
					break;
				case SDLK_RIGHT:
					input |= 0x80;
					std::cout << 8 << " RIGHT" << std::endl;
					break;
				}
			} else if (event.type == SDL_KEYUP) {
				std::cout << "KEYUP############" << event.key.keysym.sym << std::endl;
				switch (event.key.keysym.sym) {
				case SDLK_a:
					input &= ~0x1;
					std::cout << 1 << std::endl;
					break;
				case SDLK_b:
					input &= ~0x2;
					std::cout << 2 << std::endl;
					break;
				case SDLK_s:
					input &= ~0x4;
					std::cout << 3 << std::endl;
					break;
				case SDLK_SPACE:
					input &= ~0x8;
					std::cout << 4 << std::endl;
					break;
				case SDLK_UP:
					input &= ~0x10;
					std::cout << 5 << std::endl;
					break;
				case SDLK_DOWN:
					input &= ~0x20;
					std::cout << 6 << std::endl;
					break;
				case SDLK_LEFT:
					input &= ~0x40;
					std::cout << 7 << std::endl;
					break;
				case SDLK_RIGHT:
					input &= ~0x80;
					std::cout << 8 << std::endl;
					break;
				}
			} else if(event.type == SDL_DROPFILE) { // a file was droppen on the window
				dir = event.drop.file;
				SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION,
					"Loading ROMs by dropping not implemented yet :(",
					dir,
					mainWindow);
				SDL_free(dir);
			}
		}
		c1.pushButton(input);
		start2 = clock()/double(CLOCKS_PER_SEC) * 1000;
		double duration = clock();
		double duration2 = clock();
		nes.stepSeconds(0.016f);
		//std::cout << "Nes took: " << (clock() - start2) / double(CLOCKS_PER_SEC) * 1000 << std::endl;
		duration = (clock() - duration) / ((double)CLOCKS_PER_SEC) * 1000;
		//std::cout << "Duration: " << duration<< std::endl;
		if (duration < 16.6667) {
			SDL_Delay(16.6667 - duration);
			//std::cout << "test  " << duration << std::endl;
		}
		std::cout << "\rFPS: " << std::setprecision(3) << 1.0 / ((clock() - duration2) / ((double)CLOCKS_PER_SEC)) 
			<< "/" << std::setprecision(4) << 1.0 / (duration / 1000.0);// << std::endl;
		frame++;
		//std::cout << frame << std::endl;
		//std::cout << "Frame took: " << (clock() - start) / double(CLOCKS_PER_SEC) * 1000 << std::endl;
	}

	
	SDL_DestroyWindow(patternTableWindow);
	SDL_DestroyWindow(mainWindow);
	SDL_Quit();
	std::cin.get();

	delete[] fileBuffer;
	return 0;
}