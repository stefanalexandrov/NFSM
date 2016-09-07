#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>
#include <memory>
#include "TransformAlgorithm.h"
#include "Logger.h"
#include "Exceptions.h"

struct reg_decom {
	reg_decom(char a, std::string::iterator p) : ch(a), pos(p) {};
	char ch;
	std::string::iterator pos;
};

class Thompsons : public TransformAlgorithm {
public:
	Thompsons(std::string regexpr);
	Thompsons(const Thompsons&) = delete;
	Thompsons& operator=(const Thompsons&) = delete;
	StateCouple transform(); //inteface
	std::wstring get_log() const { return m_output_ws; }
	std::shared_ptr<State> get_states() const { return m_states; }
	int get_number_of_states() const { return m_s_id; }
	inline void set_logging(bool b) { m_logging = b; }
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
	Logger& m_logger;
	bool m_logging;
};