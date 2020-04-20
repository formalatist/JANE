#pragma once

#include "UIState.h"
#include "UIElement.h"
#include "Button.h"
#include "ROMInfo.h"
#include "ROMLoader.h"

namespace UI {
	class ROMLibraryState : public UIState {
	public:
		ROMLibraryState(UIFSM *UIFSM_, std::vector<ROMInfo> &ROMInfos_, const NES &nes_, ROMLoader &ROMLoader_);

		void update(float d) override;
		void draw(SDL_Renderer *renderer, int scale) override;

	private:
		std::vector<ROMInfo> ROMInfos;
		NES nes;
	};
}
