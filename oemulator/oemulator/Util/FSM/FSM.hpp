#pragma once
#include <map>
#include <string>

class SDL_Renderer;
class State;

template <class stateT>
class FSM {

public:
	void setTransitions(std::map<std::string, stateT*> transitions_);
	void changeState(std::string action);

	stateT *getState();

private:
	stateT *currentState;
	std::map<std::string, stateT*> transitions;

};

//implementation 
template <class stateT>
void FSM<stateT>::setTransitions(std::map<std::string, stateT*> transitions_)
{
	transitions = transitions_;
}

template <class stateT>
void FSM<stateT>::changeState(std::string action)
{
	if (transitions.find(action) != transitions.end()) {
		currentState = transitions[action];
	}
}

template<class stateT>
stateT *FSM<stateT>::getState()
{
	return currentState;
}