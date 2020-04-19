#pragma once

#include "UIState.h"
#include "UIElement.h"
#include "Button.h"

namespace UI {
	class ROMLibraryState : public UIState {
	public:
		ROMLibraryState(UIFSM *UIFSM_);

		void update(float d) override;
		void draw(SDL_Renderer *renderer, int scale) override;
	};
}
