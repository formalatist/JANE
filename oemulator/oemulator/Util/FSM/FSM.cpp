#include "FSM.h"

template <class stateT>
FSM<stateT>::FSM(std::map<std::string, stateT> transitions_)
{
	transitions = transitions_;
}

template <class stateT>
void FSM<stateT>::changeState(std::string action)
{
	if (transitions.find(action) != transitions.end()) {
		currentState = &(transitions[action]);
	}
}

template<class stateT>
stateT *FSM<stateT>::getState()
{
	return &currentState;
}

