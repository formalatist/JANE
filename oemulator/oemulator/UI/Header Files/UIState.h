#pragma once
#include <SDL.h>

#include "../../Util/FSM/FSM.h"


namespace UI {
	class UIState {
	public:
		typedef FSM<UI::UIState> UIFSM;

		UIState(FSM<UIState> *FSM_) : FSM(FSM_) {}

		virtual void update(float d) = 0;
		virtual void draw(SDL_Renderer *renderer) = 0;

	private:
		const FSM<UIState> *FSM;
	};
}