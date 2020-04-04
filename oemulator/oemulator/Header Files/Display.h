#pragma once
#include <SDL.h>

class Display {

public:

	Display(const char *windowName, int width, int height);

	void updateScreen(int *pixels);

	void setScale(int newScale);

	~Display();

private:
	SDL_Surface *windowSurface;
	SDL_Window *window;

	int width;
	int height;
	int scale = 1;
};