#pragma once

#include <vector>
#include <map>
#include "Transition.h"

class State {
public:
	State(int id, bool initial = false, bool finals = false);
	State();
	void set_transition(int id, char symbol, State * in);
	inline void set_final(bool b) { m_final_state = b; }
	inline void set_initial(bool b) { m_init_state = b; }
	inline bool is_final() const { return m_final_state; }
	inline bool is_initial() const { return m_init_state; }
	inline bool is_empty() const { return m_empty; }
	inline void set_empty(bool b) { m_empty = b; }
	inline void set_id(int id) { m_id = id; }
	inline int get_id() const { return m_id; }
	inline std::vector<Transition>& get_out() { return m_out; }
	inline std::vector<Transition>& get_in() { return m_in; }
private:
	std::vector<Transition> m_out;
	std::vector<Transition> m_in; // as of now only for optimization purposes
	int m_id;
	bool m_init_state;
	bool m_final_state;
	bool m_empty;
};

struct StateCouple {
	StateCouple(State * s_init, State * s_final) : m_init(s_init), m_final(s_final) {};
	StateCouple() {};
	State * m_init;
	State * m_final;
};

typedef std::map<char, StateCouple> SymbolAndNFSMmap;
typedef std::pair<char, StateCouple> SymbolAndNFSMpair;
typedef std::map<std::string, StateCouple> SymbolAndNFSMmapS;
typedef std::pair<std::string, StateCouple> SymbolAndNFSMpairS;

