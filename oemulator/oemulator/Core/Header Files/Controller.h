#pragma once

#include "NES.h"
#include "InputState.h"

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

	void update(const Input::InputState& input);
	void setKeyMap(std::string filePath);

	//keyboard input
	void onKeyDown(SDL_Keycode keycode);
	void onKeyUp(SDL_Keycode keycode);
	//controller input
	void onGameControllerDown(Uint8 btn);
	void onGameControllerUp(Uint8 btn);
	void onGameControllerAxisMotion(Uint8 axis_, Sint16 value);

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
	//TODO: read from file? maybe controller buttons should just be hardcoded?
	std::map<SDL_GameControllerButton, byte> gameControllerButtonMap = {
		{ SDL_CONTROLLER_BUTTON_A, 0x1 },
		{ SDL_CONTROLLER_BUTTON_B, 0x2 },
		{ SDL_CONTROLLER_BUTTON_BACK, 0x4 },
		{ SDL_CONTROLLER_BUTTON_START, 0x8 },
		{ SDL_CONTROLLER_BUTTON_DPAD_UP, 0x10 },
		{ SDL_CONTROLLER_BUTTON_DPAD_DOWN, 0x20 },
		{ SDL_CONTROLLER_BUTTON_DPAD_LEFT, 0x40 },
		{ SDL_CONTROLLER_BUTTON_DPAD_RIGHT, 0x80 },
	};
	const float AXIS_MAX_VALUE = 32767;
	const float AXIS_DEADZONE = 0.2f;
	enum AxisDirection {positive, negative};
	std::map<SDL_GameControllerAxis, std::map<AxisDirection, byte>> gameControllerAxisMap = {
		{ SDL_CONTROLLER_AXIS_LEFTX,{ { positive,0x80 },{ negative,0x40 }, } },
		{ SDL_CONTROLLER_AXIS_LEFTY,{ { positive,0x20 },{ negative,0x10 }, } },
	};

	void pushButton(byte val);
	void releaseButton(byte val);
	void releaseAll();
};