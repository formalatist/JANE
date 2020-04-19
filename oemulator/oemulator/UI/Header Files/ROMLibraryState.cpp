#include <iostream>

#include "ROMLibraryState.h"

UI::ROMLibraryState::ROMLibraryState(UIFSM * UIFSM_) : UIState(UIFSM_)
{
	int libBtnSize = 48;
	int numCols = 4;
	int paddingBetweenCols = (256 - numCols*libBtnSize)/(numCols+1);

	int numROMs = 30;
	for (int i = 0; i < numROMs; i++) {
		int x = (i%numCols)*(libBtnSize + paddingBetweenCols) + paddingBetweenCols;
		int y = (i / numCols)*(libBtnSize + paddingBetweenCols) + paddingBetweenCols;
		auto btn = new Button(SDL_Rect{ x, y, libBtnSize, libBtnSize }, SDL_Color{ 200,50,50,255 },
			[i, this]() { 
				std::cout << "You clicked a button " << i << std::endl; 
				this->FSM->changeState("MainMenu");
			});
		UIElements.push_back(btn);
	}
}

//TODO: input should probably be passed to the states to make it more obvious that
//handling inputs for a specific state happens here, having it be global makes it unclear
void UI::ROMLibraryState::update(float d)
{
	if (input.scrollwheelY != 0) {
		//we scrolled, update all the button positions
		for (auto e : UIElements) {
			e->rect.y += input.scrollwheelY*5;
		}
	}
	for (auto e : UIElements) {
		e->update(d);
	}
}

void UI::ROMLibraryState::draw(SDL_Renderer * renderer, int scale)
{
	for (auto e : UIElements) {
		e->draw(renderer, scale);
	}
}
