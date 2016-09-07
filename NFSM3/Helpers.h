#pragma once
#include <string>
#include <vector>
#include <memory>
#include "State.h"

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
State* find_initial_state(State* Array, int ArrSize);
State* find_final_state(State* Array, int ArrSize);
std::unique_ptr<State, std::default_delete<State[]>> copy_nfsm(State * init, State * final_s, int ArrSize, int NStates);
