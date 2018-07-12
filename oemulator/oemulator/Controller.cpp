#include "Controller.h"

Controller::Controller(){}

void Controller::setMemory(Memory * mem_)
{
	mem = mem_;
}

void Controller::pushButton(byte val)
{
	state |= val;
}

void Controller::releaseButton(byte val)
{
	state &= ~val;
}

byte Controller::readController()
{
	byte val = state >> currentBit;
	currentBit = (currentBit + strobe) % 8;
	return val;
}

void Controller::writeController(byte val)
{
	strobe = val & 1;
}
