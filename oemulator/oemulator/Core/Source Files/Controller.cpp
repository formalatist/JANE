#include "Controller.h"

Controller::Controller(){
	gameController = NULL;
	for (int i = 0; i < SDL_NumJoysticks(); i++) {
		if (SDL_IsGameController(i)) {
			gameController = SDL_GameControllerOpen(i);
			if (gameController) {
				std::cout << "Opened GameController" << SDL_GameControllerName(gameController) << std::endl;
			}
		}
	}
	if (gameController == NULL) {
		std::cout << "could not open controller" << std::endl;
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

void Controller::onGameControllerAxisMotion(Uint8 axis_, Sint16 value)
{
	//map value to [-1,1]
	float v = value / AXIS_MAX_VALUE;
	std::cout << v << std::endl;
	SDL_GameControllerAxis axis = static_cast<SDL_GameControllerAxis>(axis_);
	AxisDirection dir = v > 0 ? positive : negative;
	if (gameControllerAxisMap.find(axis) != gameControllerAxisMap.end()) { //we have a mapping for this axis
		if (abs(v) > AXIS_DEADZONE) {//we are outside the deadzone so we count it as input
			pushButton(gameControllerAxisMap[axis][dir]);
		}
		else {
			releaseButton(gameControllerAxisMap[axis][dir]);
			if (v == 0) {
				releaseButton(gameControllerAxisMap[axis][positive]);
				releaseButton(gameControllerAxisMap[axis][negative]);
			}
		}
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
