#pragma once
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <iostream>
#include <set>
#include <memory>

const int MAX_NUMBER_OF_STATES = 10000;
const int MAX_NUMBER_OF_NFSM_COPIES = 1000;
const char LAMBDA_CH = '§'; //character to represent a lambda-transition
const char BETA_CH = '#'; //character to represent a beta-transition
						  //(like lambda-transition, but consumes one symbol) for "." metachar
const char BRACKET_DELIM = '$';
const char OR_DELIM = '%';

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

class TransformAlgorithm {
public:
	virtual StateCouple transform() = 0;
	virtual std::wstring get_log() const = 0;
	virtual std::shared_ptr<State> get_states() const = 0;
	virtual int get_number_of_states() const = 0;
	virtual ~TransformAlgorithm() {};
};
class NFSMSaver {
public:
	virtual int save(std::string, State*, int) = 0;
	virtual ~NFSMSaver() {}
};
class Optimizer {
public:
	virtual void optimize(State* init, int m_s_id) = 0;
	virtual ~Optimizer() {}
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
	friend TransType run_lambda(RUN*, std::vector<NFSM>*, bool, std::set<int>*, int*);
	TransType RUN::transition_for_symbol(std::vector<NFSM>*, NFSM*, char, bool, int*, bool formal = false);
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
	virtual ~NFSM() {}; //should be virtual for shared_ptr
	NFSM(const NFSM&); //copy constructor
	NFSM& operator=(const NFSM&); //copy assignment
	NFSM(NFSM&&); //move constructor
	NFSM& operator=(NFSM&&); //move assignment
	friend TransformAlgorithm;
	int construct(TransformAlgorithm&);
	int save(NFSMSaver& saver);
	inline void set_out_wnd(CWnd* wn) { m_output = wn;}
	inline void set_invalid() {	m_valid = false;}
	inline bool is_valid() const {	return m_valid;}
	inline bool is_constructed() const {return m_constructed;}
	inline bool set_constructed(bool val) {m_constructed = val;}
	inline State* get_current() { return m_current; }
	inline void set_current(State* st) { m_current = st; }
	void optimize(Optimizer&);
private:
	std::shared_ptr<State> m_states; // array of all states
	State * m_current;
	bool m_constructed;
	bool m_valid;
	CWnd * m_output;
	StateCouple m_nfsm;
	int m_s_id;
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
	State * m_from; // not valid
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
class Thompsons : public TransformAlgorithm {
public:
	Thompsons(std::string regexpr);
	StateCouple transform(); //inteface
	std::wstring get_log() const { return m_output_ws; }
	std::shared_ptr<State> get_states() const { return m_states; }
	int get_number_of_states() const { return m_s_id; }
	~Thompsons() {}
private:
	void first_stage();
	void second_stage();
	void third_stage();
	void fourth_stage();
	void fifth_stage();
	void concatenate();
	StateCouple make_star_NFSM(State*, State*);
	StateCouple make_plus_NFSM(State*, State*);
	StateCouple make_question_NFSM(State*, State*);
	StateCouple make_or_NFSM(State*, State*, State*, State*);
	StateCouple make_simple_NFSM(State*, State*);
	StateCouple make_one_symbol_NFSM(char ch);
	StateCouple connect_NFSM(State*, State*, State*, State*);
	StateCouple make_bracket_NFSM(std::string::iterator, std::wstring&, char);
	StateCouple copy_nfsm(State*, State*);
	std::string read_or_forwards(std::string::iterator& it);
	std::string read_or_backwards(std::string::iterator& it);
	std::string read_bracket(std::string::iterator& it);
	std::vector<reg_decom> symbols;
	std::vector<reg_decom> meta_symbols;
	std::wstring m_output_ws;
	SymbolAndNFSMmap m_1_structure;
	SymbolAndNFSMmapS m_2_structure;
	SymbolAndNFSMmapS m_3_structure;
	SymbolAndNFSMmapS m_4_structure;
	SymbolAndNFSMmapS m_or_structure;
	int m_s_id;
	int m_t_id;
	std::string m_regexpr;
	std::shared_ptr<State> m_states; // array of all states
};
class DOTSaver : public NFSMSaver {
public:
	int save(std::string, State*, int);
	~DOTSaver() {}
};

class SuperflousStatesRemover: public Optimizer {
public:
	void optimize(State* init, int m_s_id);
	~SuperflousStatesRemover() {}
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
std::string read_subexpr_backwards(std::string::iterator&, char);
std::string read_subexpr_forwards(std::string::iterator&, char);

