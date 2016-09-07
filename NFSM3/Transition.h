#pragma once

class State;

class Transition {
public:
	Transition();
	Transition(int id, char symbol, State * from, State * in);
	int get_id() const;
	State * get_from() const;
	void set_from(State& st);
	State * get_to() const;
	void set_to(State& st);
	char get_symbol() const;
private:
	int m_id;
	State * m_from; // not valid
	State * m_in;
	char m_symbol;
};