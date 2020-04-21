#pragma once
#include "UIState.h"
#include "UIElement.h"
#include "Button.h"

namespace UI {
	class GameActiveState : public UIState {
	public:
		GameActiveState(UIFSM *UIFSM_);

		void update(float d) override;
		void draw(SDL_Renderer *renderer, int scale) override;
	};
}