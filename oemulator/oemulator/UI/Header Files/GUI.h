#pragma once
#include <SDL.h>
#include <fstream>
#include <iostream>
#include <vector>
//FIX: Update compiler to atleast c++17?
#include <experimental/filesystem>

#include "ROMLoader.h"
#include "ROMInfo.h"
#include "MainMenuState.h"
#include "ROMLibraryState.h"


class GUI {

public:
	GUI(SDL_Renderer *renderer, int width_, int height_);

	void draw();

	void showMainMenu();

	void showRebindKeys();

	std::vector<ROMInfo> ROMInfos;

	NES nes;

private:
	SDL_Renderer *renderer;
	int width;
	int height;
};