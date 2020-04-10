#pragma once
#include <SDL.h>
#include <ImGui/imgui_sdl.h>
#include <ImGui/imgui.h>

class GUI {

public:
	GUI(SDL_Renderer *renderer_, int width_, int height_);

	void draw();

private:
	SDL_Renderer *renderer;
	int width;
	int height;
};