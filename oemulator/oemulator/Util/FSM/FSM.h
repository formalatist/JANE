#pragma once
#include <map>
#include <string>

class SDL_Renderer;
class State;

template <class stateT>
class FSM {

public:
	FSM(std::map<std::string, stateT> transitions_);

	void changeState(std::string action);

	stateT getState();

private:
	stateT *currentState;
	std::map<std::string, stateT> transitions;

};

