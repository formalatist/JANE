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

	SDL_Renderer *getRenderer();

	~Display();

private:
	SDL_Window *window;

	SDL_Surface *patternTableWindowSurface;
	SDL_Window *patternTableWindow;

	SDL_Renderer * renderer;
	SDL_Texture *texture;

	int width;
	int height;
	int scale = 1;
};