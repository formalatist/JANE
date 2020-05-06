#pragma once

#include "NES.h"

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <SDL.h>
#include <map>

class Memory;

class Controller {
public:
	Controller();

	void setMemory();

	void setKeyMap(std::string filePath);

	void onKeyDown(SDL_Keycode keycode);
	void onKeyUp(SDL_Keycode keycode);

	byte readController();
	void writeController(byte val);

private:
	//the current state of the puttons
	byte state;
	//current bit being read out by the shift register
	int currentBit;
	//toggle for the strobe, when strobe is high controller currentBit is always 0
	int strobe;
	//how keyboard keys map to nes controller buttons
	std::map<SDL_Keycode, byte> keyMap;
	//how string names translate to NES controller button values
	const std::map<std::string, byte> buttonMap = {
		{ "A", 0x1 },
		{ "B", 0x2 },
		{ "SELECT", 0x4 },
		{ "START", 0x8 },
		{ "UP", 0x10 },
		{ "DOWN", 0x20 },
		{ "LEFT", 0x40 },
		{ "RIGHT", 0x80 },
	};

	void pushButton(byte val);
	void releaseButton(byte val);
	void releaseAll();
};