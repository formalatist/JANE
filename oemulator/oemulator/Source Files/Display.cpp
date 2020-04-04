#include "Display.h"

Display::Display(const char *windowName, int width_, int height_)
{
	width = width_;
	height = height_;

	//create the actuall window
	window = SDL_CreateWindow(windowName, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		width, height,
		SDL_WINDOW_SHOWN);
	//create surface for rendering to it
	windowSurface = SDL_GetWindowSurface(window);
}

void Display::updateScreen(int *pixels)
{
	int s = 2;
	for (int x = 0; x < 512; x++) {
		for (int y = 0; y < 240; y++) {
			for (int subX = 0; subX < scale; subX++) {
				for (int subY = 0; subY < scale; subY++) {
					Uint8 *targetPixel = (Uint8*)windowSurface->pixels 
						+ (y*scale + subY)*windowSurface->pitch
						+ (x*scale + subX) * 4;
					*(Uint32 *)targetPixel = pixels[x + y * 512];
				}
			}
		}
	}
	SDL_UpdateWindowSurface(window);
}

void Display::setScale(int newScale)
{
	scale = newScale;
	SDL_SetWindowSize(window,
		width*scale, height*scale);
	SDL_FreeSurface(windowSurface);
	windowSurface = SDL_GetWindowSurface(window);
}

Display::~Display()
{
	SDL_DestroyWindow(window);
}
