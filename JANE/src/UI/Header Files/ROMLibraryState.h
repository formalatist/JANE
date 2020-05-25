#pragma once
#include <SDL_image.h>

#include "UIState.h"
#include "UIElement.h"
#include "ImageButton.h"
#include "ROMInfo.h"
#include "ROMLoader.h"

namespace UI {
	class ROMLibraryState : public UIState {
	public:
		ROMLibraryState(UIFSM *UIFSM_, std::vector<ROMInfo> &ROMInfos_, const NES &nes_, ROMLoader &ROMLoader_, SDL_Renderer *renderer, bool &emulatorRunning_);

		void update(const Input::InputState& input, float d) override;
		void draw(SDL_Renderer *renderer, int scale) override;

	private:
		std::vector<ROMInfo> ROMInfos;
		//all the thumbnails for the ROMs in the library (listed in ROMInfos). 
		//These are used for the buttons
		std::vector<SDL_Texture*> textures;
		NES nes;
	};
}
