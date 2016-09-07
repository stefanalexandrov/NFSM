#include "stdafx.h"

#include "State.h"

State::State() {
	m_id = 0;
	m_init_state = false;
	m_final_state = false;
	m_empty = true;
}

State::State(int id, bool initial, bool finals) {
	m_id = id;
	m_init_state = initial;
	m_final_state = finals;
	m_empty = false;
}
void State::set_transition(int id, char symbol, State * in) {
	m_out.push_back(Transition(id, symbol, this, in));
	in->m_in.push_back(Transition(id, symbol, this, in));
}
