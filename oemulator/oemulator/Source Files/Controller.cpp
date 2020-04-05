#include "Controller.h"

Controller::Controller(){
	keyMap[SDLK_a] = 0x1;
	keyMap[SDLK_b] = 0x2;
	keyMap[SDLK_s] = 0x4;
	keyMap[SDLK_SPACE] = 0x8;
	keyMap[SDLK_UP] = 0x10;
	keyMap[SDLK_DOWN] = 0x20;
	keyMap[SDLK_LEFT] = 0x40;
	keyMap[SDLK_RIGHT] = 0x80;
}

void Controller::setMemory(Memory * mem_)
{
	mem = mem_;
}

void Controller::onKeyDown(SDL_Keycode keycode)
{
	if (keyMap.find(keycode) != keyMap.end()) {
		pushButton(keyMap[keycode]);
	}
}

void Controller::onKeyUp(SDL_Keycode keycode)
{
	if (keyMap.find(keycode) != keyMap.end()) {
		releaseButton(keyMap[keycode]);
	}
}

void Controller::pushButton(byte val)
{
	state |= val;
}

void Controller::releaseButton(byte val)
{
	state &= ~val;
}

void Controller::releaseAll()
{
	state = 0;
}

byte Controller::readController()
{
	byte val = (state >> currentBit)&1;
	currentBit++;
	//currentBit %= 8;
	if (strobe) {
		currentBit = 0;
	}
	//std::cout << "READING C: " << (int)state << "  val: " << (int)val << std::endl;
	return val;
}

void Controller::writeController(byte val)
{
	strobe = val & 1;
	if (val & 1) {
		currentBit = 0;
	}
}
