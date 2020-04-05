#pragma once

#include "NES.h"

#include <SDL.h>
#include <map>

class Memory;

class Controller {
public:
	Controller();

	void setMemory(Memory* mem_);

	void onKeyDown(SDL_Keycode keycode);
	void onKeyUp(SDL_Keycode keycode);

	void pushButton(byte val);
	void releaseButton(byte val);
	void releaseAll();

	byte readController();
	void writeController(byte val);

private:
	//the memory we are cocntected to
	Memory* mem;
	//the current state of the puttons
	byte state;
	//current bit being read out by the shift register
	int currentBit;
	//toggle for the strobe, when strobe is high controller currentBit is always 0
	int strobe;
	//how keyboard keys map to nes controller buttons
	std::map<SDL_Keycode, byte> keyMap;
};