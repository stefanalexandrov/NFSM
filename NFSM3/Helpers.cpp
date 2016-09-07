#include "stdafx.h"
#include "Helpers.h"
#include "Exceptions.h"

std::wstring read_output_wnd(CWnd * wnd) {
	int output_size = wnd->GetWindowTextLength();
	std::wstring output_ws;
	LPTSTR output = new TCHAR[output_size + 1];
	if (output_size > 0) {
		wnd->GetWindowTextW(output, output_size + 1);
		output_ws = output;
	}
	delete[] output;
	return output_ws;
}
std::string read_subexpr_backwards(std::string::iterator& it, char delimeter) {
	std::string str = "";
	str = *it;
	it--;
	while (*it != delimeter && is_numeric(*it)) {
		str.insert(str.begin(), *it);
		it--;
	}
	str.insert(str.begin(), *it);
	return str;
}
std::string read_subexpr_forwards(std::string::iterator& it, char delimeter) {
	std::string str = "";
	str = *it;
	it++;
	while (*it != delimeter && is_numeric(*it)) {
		str += *it;
		it++;
	}
	str += *it;
	return str;
}
bool is_star_plus_quest(char ch) {
	switch (ch) {
	case '*': return true;
	case '+': return true;
	case '?': return true;
	default: return false;
	}
}
bool is_numeric(char ch) {
	switch (ch) {
	case '0': return true;
	case '1': return true;
	case '2': return true;
	case '3': return true;
	case '4': return true;
	case '5': return true;
	case '6': return true;
	case '7': return true;
	case '8': return true;
	case '9': return true;
	default: return false;
	}
}
bool is_bracket(char ch) {
	switch (ch) {
	case ')': return true;
	case '(': return true;
	default: return false;
	}
}
bool is_meta_char_nb(char ch) { // without ()
	for (std::vector<char>::iterator j = METACHAR_.begin(); j != METACHAR_.end(); ++j)
		if (ch == *j && ch != '(' && ch != ')' && ch != '|')
			return true;
	return false;
}
State * find_initial(std::vector<State> * s) {
	for (std::vector<State>::iterator i = s->begin(); i != s->end(); i++)
		if ((*i).is_initial())
			return &(*i);
}
State * find_final(std::vector<State> * s) {
	for (std::vector<State>::iterator i = s->begin(); i != s->end(); i++)
		if ((*i).is_final())
			return &(*i);
}
bool is_meta_char(char ch) {
	for (std::vector<char>::iterator j = METACHAR_.begin(); j != METACHAR_.end(); ++j)
		if (ch == *j)
			return true;
	return false;
}
State* find_initial_state(State* Array, int ArrSize) {
	for (int i = 0; i < ArrSize; i++)
		if (Array[i].is_initial() == true)
			return &Array[i];
}
State* find_final_state(State* Array, int ArrSize) {
	for (int i = 0; i < ArrSize; i++)
		if (Array[i].is_final() == true)
			return &Array[i];
}
std::unique_ptr<State, std::default_delete<State[]>> copy_nfsm(State * init, State * final_s,
	int ArrSize, int NStates) {
	if (!init || !final_s) throw Nullptr("copy_nfsm: null pointer passed as an argument");
	std::unique_ptr<State, std::default_delete<State[]>> NewStates{ new State[NStates] };
	//first copy initial state
	State* states = NewStates.get();
	int s_id = 0;
	states[s_id] = *init;
	init->set_empty(true);
	states[s_id].set_id(s_id);
	s_id++;
	// then copy final state
	states[s_id] = *final_s;
	final_s->set_empty(true);
	states[s_id].set_id(s_id);
	states[s_id].set_final(true);
	int final_id = s_id;
	StateCouple copy = StateCouple(&states[s_id - 1], &states[s_id]);
	s_id++;

	// now copy states in between
	int done = 0;
	int done_check = 0;
	std::vector<State *> current;
	std::vector<State *> current_new;
	std::map<int, int> book; // first int is id of copied state and second int is id of the copy
	current.push_back(&states[s_id - 2]); // the initial state is current
	while (1) {
		done_check = done;
		for (auto j = current.begin(); j != current.end(); j++) {
			for (auto i = (*j)->get_out().begin(); i != (*j)->get_out().end(); i++) {
				if (i->get_to() == final_s) {
					i->set_to(states[final_id]);
					//current_new.push_back(&m_states[m_s_id]);
					done++;
				}
				else if (i->get_to() != NULL) {
					if (book.find(i->get_to()->get_id()) != book.end()) {
						State & from = states[book.at(i->get_to()->get_id())];
						i->set_to(from);
						from.get_in().push_back(Transition(i->get_id(), i->get_symbol(), *j, &from));
					}
					else {
						states[s_id] = *(i->get_to()); // copy state
						i->get_to()->set_empty(true);
						// book keeping
						book[i->get_to()->get_id()] = s_id;

						states[s_id].set_id(s_id);
						i->set_to(states[s_id]); // make transition to the newly copied state
						states[s_id].get_in().clear();
						states[s_id].get_in().push_back(Transition(i->get_id(), i->get_symbol(), *j, &states[s_id]));
						current_new.push_back(&states[s_id]);
						s_id++;
					}
					done++;
				}
			}

		}
		if (done_check == done)
			break;

		current = current_new;
		current_new.clear();
	}
	return NewStates;
}