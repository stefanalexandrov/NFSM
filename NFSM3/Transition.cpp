#include "stdafx.h"
#include "Transition.h"

Transition::Transition() {}
Transition::Transition(int id, char symbol, State * from, State * in) {
	m_id = id;
	m_from = from;
	m_in = in;
	m_symbol = symbol;
}
int Transition::get_id() const { 
	return m_id; 
}
State * Transition::get_from() const { 
	return m_from; 
}
void Transition::set_from(State& st) { 
	m_from = &st; 
}
State * Transition::get_to() const { 
	return m_in; 
}
void Transition::set_to(State& st) { 
	m_in = &st; 
}
char Transition::get_symbol() const { 
	return m_symbol; 
}

