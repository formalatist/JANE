#include "Display.h"

Display::Display(const char *windowName, int width_, int height_)
{
	SDL_Init(SDL_INIT_EVERYTHING);
	width = width_;
	height = height_;

	//create the actuall window
	window = SDL_CreateWindow(windowName, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		width, height,
		SDL_WINDOW_SHOWN);
	
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	//create texture for rendering to it
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_BGRA32, SDL_TEXTUREACCESS_TARGET, width, height);
	
	ImGui::CreateContext();
	ImGuiSDL::Initialize(renderer, width, height);
}

void Display::updateScreen(int *pixels)
{
	SDL_UpdateTexture(texture, NULL, pixels, 256*4);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	
	ImGui::NewFrame();

	ImGui::ShowDemoWindow();

	ImGui::Render();
	ImGuiSDL::Render(ImGui::GetDrawData());

	SDL_RenderPresent(renderer);
}

void Display::setScale(int newScale)
{
	scale = newScale;
	SDL_SetWindowSize(window,
		width*scale, height*scale);
}

//TODO: properly implement
void Display::showPatternTable(unsigned char *ppuMemory)
{
	patternTableWindow = SDL_CreateWindow("PatternTable", 0, 0,
		1024, 512,
		SDL_WINDOW_SHOWN);

	patternTableWindowSurface = SDL_GetWindowSurface(patternTableWindow);
	int colors[4] = { 0x666666, 0xFFCCC5, 0x1412A7, 0xB53120 };
	for (int x = 0; x < 128; x++) {
		for (int y = 0; y < 128; y++) {
			int col = x / 8;
			int row = y / 8;
			int address1 = (y % 8) | (1 << 3) | (col << 4 & 0b11110000) | (row << 8 & 0xF00);
			int address2 = (y % 8) | (col << 4 & 0b11110000) | (row << 8 & 0xF00);
			int xOffset = 7 - x % 8;
			int color = (ppuMemory[address1] >> xOffset & 1)
				+ 2 * (ppuMemory[address2] >> xOffset & 1);
			color = colors[color];
			for (int x1 = 0; x1 < 4; x1++) {
				for (int y1 = 0; y1 < 4; y1++) {
					Uint8 *targetPixel = (Uint8*)patternTableWindowSurface->pixels + (y * 4 + y1)*patternTableWindowSurface->pitch
						+ (x * 4 + x1) * 4;
					*(Uint32 *)targetPixel = color;
				}
			}
		}
	}

	int colors2[4] = { 0, 0x48CDDE, 0xB71E7B, 0x4240FF };
	for (int x = 128; x < 256; x++) {
		for (int y = 0; y < 128; y++) {
			int col = x / 8;
			int row = y / 8;
			int address1 = (y % 8) | (1 << 3) | (col << 4 & 0b11110000) | (row << 8 & 0xF00) | 0x1000;
			int address2 = (y % 8) | (col << 4 & 0b11110000) | (row << 8 & 0xF00) | 0x1000;
			int xOffset = 7 - x % 8;
			int color = (ppuMemory[address1] >> xOffset & 1)
				+ 2 * (ppuMemory[address2] >> xOffset & 1);
			color = colors2[color];
			for (int x1 = 0; x1 < 4; x1++) {
				for (int y1 = 0; y1 < 4; y1++) {
					Uint8 *targetPixel = (Uint8*)patternTableWindowSurface->pixels + (y * 4 + y1)*patternTableWindowSurface->pitch
						+ (x * 4 + x1) * 4;
					*(Uint32 *)targetPixel = color;
				}
			}
		}
	}

	SDL_UpdateWindowSurface(patternTableWindow);
}

Display::~Display()
{
	SDL_DestroyWindow(window);
	SDL_DestroyWindow(patternTableWindow);
	SDL_DestroyTexture(texture);
}
