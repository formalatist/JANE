#pragma once
#include <SDL.h>
#include <vector>

#include "../../Util/FSM/FSM.hpp"
#include "UIElement.h"


namespace UI {
	class UIState {
	public:
		typedef FSM<UI::UIState> UIFSM;

		UIState(FSM<UIState> *FSM_) : FSM(FSM_) { UIElements = std::vector<UIElement*>(); }

		virtual void update(float d) = 0;
		virtual void draw(SDL_Renderer *renderer, int scale) = 0;

	protected:
		FSM<UIState> *FSM;
		std::vector<UIElement*> UIElements;
	};
}