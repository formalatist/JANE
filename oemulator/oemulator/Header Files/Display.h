#pragma once
#include <SDL.h>

class Display {

public:

	Display(const char *windowName, int width, int height);

	void updateScreen(int *pixels);

	void setScale(int newScale);

	//TODO: Properly implement, or move to debugger class
	void showPatternTable(unsigned char *ppuMemory);

	void hidePatternTable();

	~Display();

private:
	SDL_Surface *windowSurface;
	SDL_Window *window;

	SDL_Surface *patternTableWindowSurface;
	SDL_Window *patternTableWindow;

	int width;
	int height;
	int scale = 1;
};