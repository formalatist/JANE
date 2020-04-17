#pragma once
#include "../../Util/FSM/FSM.h"

typedef FSM<UIState> UIFSM;

class UIState {

public:
	UIState(FSM<UIState> *FSM_) : FSM(FSM_) {}

	virtual void update(float d) = 0;
	virtual void draw() = 0;
	
private:
	const FSM<UIState> *FSM;
};