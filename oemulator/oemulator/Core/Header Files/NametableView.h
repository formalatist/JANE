#pragma once
#include "NES.h"

class NametableView {
public:
	NametableView(const NES &nes_);

	void draw();

private:
	SDL_Window *window;
	SDL_Surface *nametableS;
	const NES &nes;

	int width = 256 * 2;
	int height = 240 * 2;
	int numRows = 30;
	int numColumns = 32;
};