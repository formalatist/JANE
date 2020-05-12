#include "Controller.h"

Controller::Controller(){
}


void Controller::update(const Input::InputState & input)
{
	//clear all input
	releaseAll();
	//set input based on whats being held this frame
	for (const auto& value : keyMap) {
		if (input.isKeyHeld(value.first)) {
			pushButton(value.second);
		}
	}
	for (const auto& value : gameControllerButtonMap) {
		if (input.isControllerButtonHeld(value.first)) {
			pushButton(value.second);
		}
	}
	for (const auto& value : gameControllerAxisMap) {
		for (const auto& dirToByteMap : value.second) {
			if (input.isAxisDirectionHeld(value.first, dirToByteMap.first)) {
				pushButton(dirToByteMap.second);
			}
		}
	}
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

void Controller::onGameControllerDown(Uint8 btn)
{
	if (gameControllerButtonMap.find(static_cast<SDL_GameControllerButton>(btn)) != gameControllerButtonMap.end()) {
		pushButton(gameControllerButtonMap[static_cast<SDL_GameControllerButton>(btn)]);
	}
}

void Controller::onGameControllerUp(Uint8 btn)
{
	if (gameControllerButtonMap.find(static_cast<SDL_GameControllerButton>(btn)) != gameControllerButtonMap.end()) {
		releaseButton(gameControllerButtonMap[static_cast<SDL_GameControllerButton>(btn)]);
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
