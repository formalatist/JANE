#include "GUI.h"

GUI::GUI(SDL_Renderer * renderer_, int width_, int height_)
{
	renderer = renderer_;
	width = width_;
	height = height_;

	ImGui::CreateContext();
	ImGuiSDL::Initialize(renderer, width, height);
}

void GUI::draw()
{
	ImGui::NewFrame();

	ImGui::ShowDemoWindow();

	ImGui::Render();
	ImGuiSDL::Render(ImGui::GetDrawData());

	SDL_RenderPresent(renderer);
}
