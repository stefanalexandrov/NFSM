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

struct StateCouple;

typedef std::pair<char, StateCouple> SymbolAndNFSMpair;
typedef std::map<std::string, StateCouple> SymbolAndNFSMmapS;
typedef std::pair<std::string, StateCouple> SymbolAndNFSMpairS;

class State;

struct StateCouple {
	StateCouple(State * s_init, State * s_final, std::string str, SymbolAndNFSMmapS* container) :
		m_init(s_init), m_final(s_final), m_str(str), m_container(container) {};
	StateCouple(): m_init(nullptr), m_final(nullptr), m_str(""), m_container(nullptr) {};
	StateCouple(const StateCouple& st): m_init(st.m_init), m_final(st.m_final),
		m_str(st.m_str), m_container(st.m_container), m_states(st.m_states) {}
	StateCouple& operator=(const StateCouple& st) {
		m_init = st.m_init;
		m_final = st.m_final;
		m_str = st.m_str;
		m_container = st.m_container;
		m_states = st.m_states;
		return *this;
	}
	
	State * m_init;
	State * m_final;
	std::string m_str;
	SymbolAndNFSMmapS* m_container;
	std::vector<State> m_states;
};



struct reg_decom {
	reg_decom(char a, std::string::iterator p) : ch(a), pos(p) {};
	char ch;
	std::string::iterator pos;
};


class NFSM;
TransType run_lambda();
class RUN {
public:
	RUN(NFSM*);
	virtual ~RUN() {
		//delete m_nfsm;
	}
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
	friend TransType RUN::make_transition(char input, bool last_ch);
	friend TransType run_lambda(RUN * obj, std::vector<NFSM> * p_nfsms, bool last_ch, std::set<int> * visited_s, int * n_invalidated);
	friend RUN;
	std::string read_or(std::string::iterator it);
	std::string read_bracket(std::string::iterator it);
	int construct();
	bool match(std::string str);
	void write_nfsm(std::string file_name);
	int simulate(int length);
	friend int RUN::formal(int length);
	friend TransType RUN::transition_for_symbol(std::vector<NFSM> * p_nfsms, NFSM * nfsm, char input, bool last_ch, int * n_inval, bool formal);
	StateCouple make_star_NFSM(std::string, SymbolAndNFSMmapS&, std::string);
	StateCouple make_plus_NFSM(std::string, SymbolAndNFSMmapS&, std::string);
	StateCouple make_question_NFSM(std::string, SymbolAndNFSMmapS&, std::string);
	StateCouple make_or_NFSM(std::string, SymbolAndNFSMmapS&, std::string, SymbolAndNFSMmapS&);
	StateCouple make_simple_NFSM(std::string, SymbolAndNFSMmapS&, std::string);
	StateCouple make_one_symbol_NFSM(std::string);
	StateCouple connect_NFSM(std::string, SymbolAndNFSMmapS&, std::string, SymbolAndNFSMmapS&);
	StateCouple make_bracket_NFSM(std::string::iterator i, std::wstring & output_ws, char type);
	StateCouple copy_nfsm(const std::string reg_expr, const SymbolAndNFSMmapS&);
	StateCouple helper_third_st(std::string left, std::string right);
	void bracket_NFSM_update(std::string sub_expr, StateCouple& new_nfsm);
	void simple_metachar_NFSM_update(std::string sub_expr, StateCouple& new_nfsm);
	std::wstring third_it_m();
	std::wstring fourth_it();
	std::wstring fifth_it();
	void clean();
	inline void set_regexpr(std::string st) {
		m_constructed = false;
		m_regexpr = st;
	}
	inline void set_out_wnd(CWnd* wn) {
		m_output = wn;
	}
	inline size_t get_size() {
		return sizeof(m_states);
	}
	inline void set_invalid() {
		m_valid = false;
	}
	inline bool is_valid() {
		return m_valid;
	}
	inline bool is_constructed() {
		return m_constructed;
	}
	void optimize();
private:
	SymbolAndNFSMmapS m_1_structure;
	SymbolAndNFSMmapS m_2_structure;
	SymbolAndNFSMmapS m_3_structure;
	SymbolAndNFSMmapS m_4_structure;
	SymbolAndNFSMmapS m_or_structure;
	SymbolAndNFSMmapS m_connect_structure;
	//std::shared_ptr<State> m_states; // array of all states
	State * m_current;
	bool m_constructed;
	bool m_valid;
	std::string m_regexpr;
	CWnd * m_output;
	int m_s_id;
	int m_t_id;

protected:
};

class Transition {
public:
	Transition();
	Transition(int id, char symbol, State * from, State * in);
	virtual ~Transition();
	friend TransType RUN::make_transition(char input, bool last_ch);
	friend TransType run_lambda(RUN * obj, std::vector<NFSM> * p_nfsms, bool last_ch, std::set<int> * visited_s, int * n_invalidated);
	friend StateCouple NFSM::copy_nfsm(const std::string reg_expr, const SymbolAndNFSMmapS&);
	friend void NFSM::write_nfsm(std::string file_name);
	friend std::wstring NFSM::third_it_m();
	friend std::wstring NFSM::fourth_it();
	friend std::wstring NFSM::fifth_it();
	friend StateCouple NFSM::helper_third_st(std::string left, std::string right);
	friend std::string NFSM::read_or(std::string::iterator it);
	friend std::string NFSM::read_bracket(std::string::iterator it);
	friend void NFSM::optimize();
	friend TransType RUN::transition_for_symbol(std::vector<NFSM> * p_nfsms, NFSM * nfsm, char input, bool last_ch, int * n_inval, bool formal);
	//friend int NFSM::formal_method();
	friend int RUN::formal(int length);
	friend void move_inputs_to(State* from, State* to);
	friend void move_outputs_to(State* from, State* to);
	friend void update_connections(State& st, const State& old);
	int get_id() const;
	void set_transition_to(State* st, int* current_id, const char symbol, State* old = nullptr);
	State * get_from_state();
	State * get_in_state();
private:
	int m_id;
	State * m_from;
	State * m_in;
	char m_symbol;
protected:

};

class State {
public:
	State(int id, bool initial = false, bool finals = false);
	State();
	//virtual ~State();
	friend NFSM;
	friend TransType RUN::make_transition(char input, bool last_ch);
	friend TransType run_lambda(RUN * obj, std::vector<NFSM> * p_nfsms, bool last_ch, std::set<int> * visited_s, int * n_invalidated);
	friend StateCouple NFSM::copy_nfsm(const std::string reg_expr, const SymbolAndNFSMmapS&);
	friend void NFSM::write_nfsm(std::string file_name);
	friend std::wstring NFSM::third_it_m();
	friend std::wstring NFSM::fourth_it();
	friend std::wstring NFSM::fifth_it();
	friend StateCouple NFSM::helper_third_st(std::string left, std::string right);
	friend std::string NFSM::read_or(std::string::iterator it);
	friend std::string NFSM::read_bracket(std::string::iterator it);
	friend void NFSM::optimize();
	friend TransType RUN::transition_for_symbol(std::vector<NFSM> * p_nfsms, NFSM * nfsm, char input, bool last_ch, int * n_inval, bool formal);
	//friend int NFSM::formal_method();
	friend int RUN::formal(int length);
	friend void move_inputs_to(State* from, State* to);
	friend void move_outputs_to(State* from, State* to);
	friend void update_connections(State& st, const State& old);
	friend void Transition::set_transition_to(State* st, int* current_id, char symbol, State* old);
	void set_transition(int id, char symbol, State * in);
	void set_final(bool b);
	void set_initial(bool b);
	bool is_final();
	bool is_initial();
	void set_id(int id) {
		m_id = id;
	}
private:
	std::vector<Transition> m_out;
	std::vector<Transition> m_in; // as of now only for optimization purposes
	int m_id;
	bool m_init_state;
	bool m_final_state;
	bool m_empty;
protected:

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
void move_inputs_to(State* from, State* to);
void move_outputs_to(State* from, State* to);
void update_connections(State& st, const State& old);
//int transition_for_symbol(NFSM * nfsm, char input, bool last_ch);

