#pragma once

#include "NES.h"

class Memory;

class Controller {
public:
	Controller();

	void setMemory(Memory* mem_);

	void pushButton(byte val);
	void releaseButton(byte val);

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
};