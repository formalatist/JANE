#include "Display.h"

Display::Display(const char *windowName, int width_, int height_)
{
	width = width_;
	height = height_;

	//create the actuall window
	window = SDL_CreateWindow(windowName, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		width*2, height*2,
		SDL_WINDOW_SHOWN);
	//create surface for rendering to it
	windowSurface = SDL_GetWindowSurface(window);
}

void Display::updateScreen(int *pixels)
{
	int s = 2;
	for (int x = 0; x < 512; x++) {
		for (int y = 0; y < 240; y++) {
			//original scale = 1 rendering
			/*Uint8 *targetPixel = (Uint8*)screen->pixels + (y)*screen->pitch
			+ (x) * 4;
			*(Uint32 *)targetPixel = ppu->pixels[x + y * 512];*/

			Uint8 *targetPixel = (Uint8*)windowSurface->pixels + (y*s)*windowSurface->pitch
				+ (x*s) * 4;
			*(Uint32 *)targetPixel = pixels[x + y * 512];
			/*targetPixel = (Uint8*)windowSurface->pixels + (y*s + 1)*windowSurface->pitch
				+ (x*s) * 4;
			*(Uint32 *)targetPixel = pixels[x + y * 512];
			targetPixel = (Uint8*)windowSurface->pixels + (y*s)*windowSurface->pitch
				+ (x*s + 1) * 4;
			*(Uint32 *)targetPixel = pixels[x + y * 512];
			targetPixel = (Uint8*)windowSurface->pixels + (y*s + 1)*windowSurface->pitch
				+ (x*s + 1) * 4;
			*(Uint32 *)targetPixel = pixels[x + y * 512];*/
		}
	}
	SDL_UpdateWindowSurface(window);
}

void Display::setScale(int newScale)
{
	scale = newScale;
	SDL_SetWindowSize(window,
		width*scale, height*scale);
}
