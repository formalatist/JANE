#pragma once
#include <vector>

#include "UIState.h"
#include "UIElement.h"
#include "Button.h"

namespace UI {
	class MainMenuState : public UIState {

	public:
		MainMenuState(UIFSM *UIFSM_);

		void update(float d) override;
		void draw(SDL_Renderer *renderer, int scale) override;

	private:
		std::vector<UIElement*> UIElements;
	};
}
