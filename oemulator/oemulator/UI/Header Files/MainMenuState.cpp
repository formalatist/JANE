#include "MainMenuState.h"

UI::MainMenuState::MainMenuState(UIFSM * UIFSM_) : UIState(UIFSM_)
{
	auto btn = new Button(SDL_Rect{ 64, 64, 128, 16 }, SDL_Color{ 40, 120, 240, 255 },
			[this](){
				printf("button onclick\n"); 
				this->FSM->changeState("ROMLibrary");
			});
	UIElements.push_back(btn);
}

void UI::MainMenuState::update(const Input::InputState& input, float d)
{
	for (auto e : UIElements) {
		e->update(input, d);
	}
}

void UI::MainMenuState::draw(SDL_Renderer *renderer, int scale)
{
	for (auto e : UIElements) {
		e->draw(renderer, scale);
	}
}
