#pragma once


#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <iostream>
#include <set>
#include <memory>
#include <chrono>
#include <ctime>
#include <ratio>
#include <exception>
#include "State.h"
#include "TransformAlgorithm.h"
#include "SimpleLogger.h"



enum class TransType { VALID_TRANSITION, INVALID_TRANSITION, FINAL_STATE, 
	NON_D_TRANSITION, L_TRANSITION, NOT_FINAL_LAMBDA, FINAL_STATE_LAMBDA, NOT_FINAL, ERROR_ };



class NFSMSaver {
public:
	virtual int save(std::string, State*, int) = 0;
	virtual ~NFSMSaver() {}
};
class Optimizer {
public:
	virtual void optimize(State* init, std::shared_ptr<State>, int m_s_id) = 0;
	virtual ~Optimizer() {}
};




class NFSM;
TransType run_lambda();

class RUN {
public:
	static RUN& GetInstance(NFSM* machine, CWnd* output_wnd) {
		static RUN instance(machine, output_wnd);
		return instance;
	}
	RUN(const RUN&) = delete; //copy constructor
	RUN& operator=(const RUN&) = delete; //copy assignment
	TransType make_transition(char input, bool last_ch);
	TransType make_transition(); // for empty string
	friend TransType run_lambda(RUN*, std::vector<NFSM>*, bool, std::set<int>*, int*);
	TransType RUN::transition_for_symbol(std::vector<NFSM>*, NFSM*, char, bool, int*, bool formal = false);
	int formal(int length);
private:
	RUN(NFSM* machine, CWnd* output_wnd);
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
	int construct();
	int save(NFSMSaver& saver);
	inline void set_out_wnd(CWnd* wn) { m_output = wn;}
	inline void set_invalid() {	m_valid = false;}
	inline bool is_valid() const {	return m_valid;}
	inline bool is_constructed() const {return m_constructed;}
	inline bool set_constructed(bool val) {m_constructed = val;}
	inline State* get_current() { return m_current; }
	inline void set_current(State* st) { m_current = st; }
	inline void set_logging(bool b) { m_logging = b; }
	inline void set_reg_expr(std::string str) { m_regexpr = str; }
	void optimize(Optimizer&);
private:
	std::shared_ptr<State> m_states; // array of all states
	State * m_current;
	std::string m_regexpr;
	bool m_constructed;
	bool m_valid;
	CWnd * m_output;
	StateCouple m_nfsm;
	int m_s_id;
	Logger& m_logger;
	bool m_logging;
};



class DOTSaver : public NFSMSaver {
public:
	int save(std::string, State*, int);
	~DOTSaver() {}
};

class SuperflousStatesRemover: public Optimizer {
public:
	SuperflousStatesRemover() : m_logger{ SimpleLogger::GetInstance() }, m_logging{false} {}
	SuperflousStatesRemover(const SuperflousStatesRemover&) = delete;
	SuperflousStatesRemover& operator=(const SuperflousStatesRemover&) = delete;
	void optimize(State* init, std::shared_ptr<State>, int m_s_id);
	inline void set_logging(bool b) { m_logging = true; }
	~SuperflousStatesRemover() {}
private:
	Logger& m_logger;
	bool m_logging;
};



