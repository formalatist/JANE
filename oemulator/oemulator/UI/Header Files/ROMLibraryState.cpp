#include <iostream>

#include "ROMLibraryState.h"

UI::ROMLibraryState::ROMLibraryState(UIFSM * UIFSM_, std::vector<ROMInfo> & ROMInfos_, const NES &nes_, ROMLoader &ROMLoader_, SDL_Renderer *renderer, bool &emulatorRunning_) : UIState(UIFSM_)
{
	nes = nes_;

	int libBtnSize = 48;
	int numCols = 4;
	int paddingBetweenCols = (256 - numCols*libBtnSize)/(numCols+1);
	ROMInfos = ROMInfos_;

	textures = std::vector<SDL_Texture*>();

	int numROMs = ROMInfos.size();
	for (int i = 0; i < numROMs; i++) {
		int x = (i%numCols)*(libBtnSize + paddingBetweenCols) + paddingBetweenCols;
		int y = (i / numCols)*(libBtnSize + paddingBetweenCols) + paddingBetweenCols;
		SDL_Surface *s = IMG_Load(ROMInfos_[i].thumbnailPath.c_str());
		SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, s);
		SDL_FreeSurface(s);
		textures.push_back(tex);
		std::cout << "Loaded tex from: " << ROMInfos_[i].thumbnailPath.c_str() << std::endl;
		auto btn = new ImageButton(SDL_Rect{ x, y, libBtnSize, libBtnSize }, textures[i],
			[i, this, &nes_, &ROMLoader_, &ROMInfos_, &emulatorRunning_]() mutable{ 
				std::cout << "You clicked a button " << i << std::endl; 
				this->FSM->changeState("Game");
				std::cout << "Loading ROM " << ROMInfos_[i].ROMName << " from " << ROMInfos_[i].ROMPath << std::endl;
				ROMLoader_.loadROMFromFile(ROMInfos_[i].ROMPath, *(nes_.cpu));
				emulatorRunning_ = true;
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
			e->rect.y += input.scrollwheelY*15;	//MAGIC NUMBER: just to make scrolling a bit 
												//faster, this functionality should be hidden inside scrollview / gridview in future
		}
	}
	for (auto e : UIElements) {
		e->update(d);
	}
}

void UI::ROMLibraryState::draw(SDL_Renderer * renderer, int scale)
{
	SDL_SetRenderDrawColor(renderer, 70,70,70,128);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BlendMode::SDL_BLENDMODE_BLEND);
	SDL_RenderFillRect(renderer, NULL);
	for (auto e : UIElements) {
		e->draw(renderer, scale);
	}
}
