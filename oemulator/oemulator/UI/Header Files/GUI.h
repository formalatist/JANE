#pragma once
#include <SDL.h>
#include <ImGui/imgui_sdl.h>
#include <ImGui/imgui.h>
#include <fstream>
#include <iostream>
//FIX: Update compiler to atleast c++17?
#include <experimental/filesystem>

#include "ROMLoader.h"
#include "ROMInfo.h"

class GUI {

public:
	GUI(SDL_Renderer *renderer_, int width_, int height_);

	void draw();

	void showMainMenu();

private:
	SDL_Renderer *renderer;
	int width;
	int height;
};