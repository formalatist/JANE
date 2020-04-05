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
  
	std::string game = "Super mario bros";
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

	//create the NES
	NES nes = NES();
	//create a controller
	Controller c1 = Controller();
	nes.connectController(&c1); //connect it
	//load a rom
	ROMLoader loader = ROMLoader(nes.memory, nes.ppuMemory);
	loader.loadROM(fileBuffer, fileSize, (*nes.cpu));
	//create the screen
	Display display = Display("NES Emulator", 256, 240);
	nes.setDisplay(&display);
	display.setScale(3);
	
	tickCounter = SDL_GetTicks();
	int frame = 0;
	int frameTime = 0;
	bool run = true;
	int start = 0;
	int start2 = 0;
	SDL_Event event;
	byte input = 0;

	char* dir;
	while (run) {
		start = clock();

		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				run = false;
			} else if (event.type == SDL_KEYDOWN) {
				c1.onKeyDown(event.key.keysym.sym);
			} else if (event.type == SDL_KEYUP) {
				c1.onKeyUp(event.key.keysym.sym);
			} else if(event.type == SDL_DROPFILE) { // a file was droppen on the window
				dir = event.drop.file;
				SDL_free(dir);
			}
		}

		start2 = clock()/double(CLOCKS_PER_SEC) * 1000;
		double duration = clock();
		double duration2 = clock();
		nes.stepSeconds(0.016f);
		//std::cout << "Nes took: " << (clock() - start2) / double(CLOCKS_PER_SEC) * 1000 << std::endl;
		duration = (clock() - duration) / ((double)CLOCKS_PER_SEC) * 1000;
		//std::cout << "Duration: " << duration<< std::endl;
		if (duration < 14.2/*16.6667*/) {
			SDL_Delay(14.2/*16.6667*/ - duration);
			//std::cout << "test  " << duration << std::endl;
		}
		std::cout << "\rFPS: " << std::setprecision(3) << 1.0 / ((clock() - duration2) / ((double)CLOCKS_PER_SEC)) 
			<< "/" << std::setprecision(4) << 1.0 / (duration / 1000.0);// << std::endl;
		frame++;
		//std::cout << frame << std::endl;
		//std::cout << "Frame took: " << (clock() - start) / double(CLOCKS_PER_SEC) * 1000 << std::endl;
	}

	SDL_Quit();
	std::cin.get();

	delete[] fileBuffer;
	return 0;
}