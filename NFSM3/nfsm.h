#pragma once
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <iostream>
#include <set>
#include <memory>

enum class TransType { VALID_TRANSITION, INVALID_TRANSITION, FINAL_STATE, 
	NON_D_TRANSITION, L_TRANSITION, NOT_FINAL_LAMBDA, FINAL_STATE_LAMBDA, NOT_FINAL, ERROR_ };

class State;

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

struct reg_decom {
	reg_decom(char a, std::string::iterator p) : ch(a), pos(p) {};
	char ch;
	std::string::iterator pos;
};


class NFSM;
TransType run_lambda();
class RUN {
public:
	RUN(NFSM* machine, CWnd* output_wnd);
	//RUN object should not be copied.
	RUN(const RUN&) = delete; //copy constructor
	RUN& operator=(const RUN&) = delete; //copy assignment
	//RUN(RUN&&) = delete; //move constructor
	//RUN& operator=(RUN&&) = delete; //move assignment
	TransType make_transition(char input, bool last_ch);
	TransType make_transition(); // for empty string
	friend TransType run_lambda(RUN * obj, std::vector<NFSM> * p_nfsms, bool last_ch, std::set<int> * visited_s, int * n_invalidated);
	TransType RUN::transition_for_symbol(std::vector<NFSM> * p_nfsms, NFSM * nfsm, char input, bool last_ch, int * n_inval, bool formal = false);
	int formal(int length);
private:
	std::vector<NFSM> m_nfsms;
	NFSM* m_nfsm;
	CWnd * m_output;
};

class State;

class NFSM {
public:
	NFSM();
	NFSM(std::string regexpr, CWnd * output);
	virtual ~NFSM(); //should be virtual for shared_ptr
	NFSM(const NFSM&); //copy constructor
	NFSM& operator=(const NFSM&); //copy assignment
	NFSM(NFSM&&); //move constructor
	NFSM& operator=(NFSM&&); //move assignment
	std::string read_or(std::string::iterator it);
	std::string read_bracket(std::string::iterator it);
	int construct();
	bool match(std::string str);
	void write_nfsm(std::string file_name);
	int simulate(int length);
	StateCouple make_star_NFSM(State * s_init, State * s_final);
	StateCouple make_plus_NFSM(State * s_init, State * s_final);
	StateCouple make_question_NFSM(State * s_init, State * s_final);
	StateCouple make_or_NFSM(State * s_init_1, State * s_final_1, State * s_init_2, State * s_final_2);
	StateCouple make_simple_NFSM(State * s_init, State * s_final);
	StateCouple make_one_symbol_NFSM(char ch);
	StateCouple connect_NFSM(State * s_init_1, State * s_final_1, State * s_init_2, State * s_final_2);
	StateCouple make_bracket_NFSM(std::string::iterator i, std::wstring & output_ws, char type);
	StateCouple copy_nfsm(State * init, State * final_s);
	StateCouple helper_third_st(std::string left, std::string right);
	std::wstring third_it_m();
	std::wstring fourth_it();
	std::wstring fifth_it();
	void clean();
	inline void set_regexpr(std::string st) {
		m_constructed = false;
		m_regexpr = st;
	}
	inline void set_out_wnd(CWnd* wn) { m_output = wn;}
	inline void set_invalid() {	m_valid = false;}
	inline bool is_valid() const {	return m_valid;}
	inline bool is_constructed() const {return m_constructed;}
	inline bool set_constructed(bool val) {m_constructed = val;}
	inline State* get_current() { return m_current; }
	inline void set_current(State* st) { m_current = st; }
	void optimize();
private:
	SymbolAndNFSMmap m_1_structure;
	SymbolAndNFSMmapS m_2_structure;
	SymbolAndNFSMmapS m_3_structure;
	SymbolAndNFSMmapS m_4_structure;
	SymbolAndNFSMmapS m_or_structure;
	std::shared_ptr<State> m_states; // array of all states
	State * m_current;
	bool m_constructed;
	bool m_valid;
	std::string m_regexpr;
	CWnd * m_output;
	int m_s_id;
	int m_t_id;
};

class Transition {
public:
	Transition();
	Transition(int id, char symbol, State * from, State * in);
	inline int get_id() const { return m_id; }
	inline State * get_from() const { return m_from; }
	inline void set_from(State& st) { m_from = &st; }
	inline State * get_to() const { return m_in;}
	inline void set_to(State& st) { m_in = &st; }
	inline char get_symbol() const { return m_symbol; }
private:
	int m_id;
	State * m_from;
	State * m_in;
	char m_symbol;
};

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
	inline void set_id(int id) { m_id = id;}
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

//helper functions

bool is_meta_char(char ch);
State * find_initial(std::vector<State> *);
State * find_final(std::vector<State> *);
bool is_numeric(char ch);
bool is_bracket(char ch);
bool is_meta_char_nb(char ch); //without ()
bool is_star_plus_quest(char ch);
std::wstring read_output_wnd(CWnd * wnd);

