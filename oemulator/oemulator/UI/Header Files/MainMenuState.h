#pragma once
#include "UIState.h"

class MainMenuState : UIState {

public:
	MainMenuState(UIFSM *UIFSM_);

	void update(float d) override;
	void draw() override;

private:

	
};