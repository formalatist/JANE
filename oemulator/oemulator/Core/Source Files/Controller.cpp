#include "Controller.h"

Controller::Controller(){
}


void Controller::setKeyMap(std::string filePath)
{
	std::ifstream fileStream(filePath);
	std::string line;
	while (std::getline(fileStream, line)) {
		std::istringstream stringStream(line);
		std::string SDLKeyName;
		std::string NESButton;
		if (stringStream >> SDLKeyName >> NESButton) {
			std::cout << "SDLKeyName: " << SDLKeyName << " NESButton: " << NESButton << " Buttonmap at: " << (int)buttonMap.at(NESButton) << std::endl;
			//TODO: there could be some error checking to prevent crashing on malformed keyMap files
			keyMap[SDL_GetKeyFromName(SDLKeyName.c_str())] = buttonMap.at(NESButton);
		}
	}
	fileStream.close();
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
