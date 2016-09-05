#pragma once

class State;

class Transition {
public:
	Transition();
	Transition(int id, char symbol, State * from, State * in);
	inline int get_id() const { return m_id; }
	inline State * get_from() const { return m_from; }
	inline void set_from(State& st) { m_from = &st; }
	inline State * get_to() const { return m_in; }
	inline void set_to(State& st) { m_in = &st; }
	inline char get_symbol() const { return m_symbol; }
private:
	int m_id;
	State * m_from; // not valid
	State * m_in;
	char m_symbol;
};