#include "stdafx.h"
#include "Helpers.h"

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