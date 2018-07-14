#include "Controller.h"

Controller::Controller(){}

void Controller::setMemory(Memory * mem_)
{
	mem = mem_;
}

void Controller::pushButton(byte val)
{
	state = val;
}

void Controller::releaseButton(byte val)
{
	state &= ~val;
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
