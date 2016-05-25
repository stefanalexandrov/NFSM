#include "stdafx.h"
#include "nfsm.h"
#include "NFSM3.h"
#include "NFSM3Dlg.h"
#include "DlgProxy.h"
#include "afxdialogex.h"



std::vector<char> METACHAR_ = { '(',')','*','+','?', '|'}; // '.' is not treated as metachar for simplisity

Transition::Transition() {}
Transition::Transition(int id, char symbol, State * from, State * in) {
	m_id = id;
	m_from = from;
	m_in = in;
	m_symbol = symbol;
}

State::State() {
	m_id = 0;
	m_init_state = false;
	m_final_state = false;
	m_empty = true;
}

State::State(int id, bool initial, bool finals) {
	m_id = id;
	m_init_state = initial;
	m_final_state = finals;
	m_empty = false;
}
void State::set_transition(int id, char symbol, State * in) {
	m_out.push_back(Transition(id, symbol, this, in));
	in->m_in.push_back(Transition(id, symbol, this, in));
}
Thompsons::Thompsons(std::string regexpr) : m_regexpr(regexpr),
m_states{ new State[MAX_NUMBER_OF_STATES], std::default_delete<State[]>() }
{
	State* states = m_states.get();
	for (int i = 0; i < MAX_NUMBER_OF_STATES; i++)
		states[i] = State(); //ititialize array with empty states	
}

RUN::RUN(NFSM* machine, CWnd* output_wnd) {
	m_nfsm = machine;
	m_nfsms.push_back(*machine);
	m_nfsms.reserve(MAX_NUMBER_OF_NFSM_COPIES);
	m_output = output_wnd;
}
NFSM::NFSM() : m_states{ nullptr } {
	m_constructed = false;
	m_valid = true;
}
NFSM::NFSM(const NFSM& nfsm) :
    m_states{ nfsm.m_states },
	m_current{ nfsm.m_current },
	m_constructed{ nfsm.m_constructed },
	m_valid{ nfsm.m_valid },
	m_output{ nfsm.m_output },
	m_s_id{ nfsm.m_s_id }
{}
NFSM& NFSM::operator=(const NFSM& nfsm) {
	m_states = m_states;
	m_current = nfsm.m_current;
	m_constructed = nfsm.m_constructed;
	m_valid = nfsm.m_valid;
	m_output = nfsm.m_output;
	m_s_id = nfsm.m_s_id; 
	return *this;
}
NFSM::NFSM(NFSM&& nfsm):
    m_states{ nfsm.m_states }, //just grab elements without copying
	m_current{ nfsm.m_current }, m_constructed{ nfsm.m_constructed },
    m_valid{ nfsm.m_valid }, m_output{ nfsm.m_output }, m_s_id{ nfsm.m_s_id }
{
	nfsm.m_states = nullptr;
	nfsm.m_current = nullptr;
}
NFSM& NFSM::operator=(NFSM&& nfsm) {
	m_states = nfsm.m_states; //just grab elements without copying
	m_current = nfsm.m_current;
	m_constructed = nfsm.m_constructed;
	m_valid = nfsm.m_valid;
	m_output = nfsm.m_output;
	nfsm.m_states = nullptr;
	nfsm.m_current = nullptr;
	m_s_id = nfsm.m_s_id;
	return *this;
}
void Thompsons::first_stage() {
	m_output_ws.append(_T(">First iteration: building NFSMs for single characters..."));
	//devide regular expression on symbols and metasymbols
	for (std::string::iterator i = m_regexpr.begin(); i != m_regexpr.end(); ++i) {
		bool is_meta = true;
		if (is_meta = is_meta_char(*i)) {
			meta_symbols.push_back(reg_decom(*i, i));
		}
		if (!is_meta)
			symbols.push_back(reg_decom(*i, i));
	}
	for (std::vector<reg_decom>::iterator i = symbols.begin(); i != symbols.end(); ++i) {
		m_1_structure.insert(SymbolAndNFSMpair(i->ch, make_one_symbol_NFSM(i->ch)));
	}
	m_output_ws.append(_T("Done! Number of NFSMs: "));
	const int N_DIGITS = 10;
	TCHAR number[N_DIGITS];
	swprintf_s(number, N_DIGITS, _T("%d \r\n"), m_1_structure.size());
	m_output_ws.append(number);
	return;
}
void Thompsons::second_stage() {
	m_output_ws.append(_T(">Second iteration: building NFSMs a single character followed by *, +, ? ... "));
	std::string expr;
	for (std::vector<reg_decom>::iterator i = meta_symbols.begin(); i != meta_symbols.end(); ++i) {
		std::string::iterator ch;
		if (i->pos != m_regexpr.begin()) {
			ch = (i->pos);
			--ch;
		}
		else {
			ch = (i->pos);
		}
		if (i->ch == '*' && *ch != ')' && !is_meta_char(*ch)) {
			// creating NFSM for <char>* regular expression
			StateCouple symbol_nfsm = m_1_structure.at(*ch); //accessing NFSM created for <char> reg expression
			expr = *(ch);
			expr += '*';
			m_2_structure.insert(SymbolAndNFSMpairS(expr, make_star_NFSM(symbol_nfsm.m_init, symbol_nfsm.m_final)));
		}
		else if ((*i).ch == '+' && *ch != ')' && !is_meta_char(*ch)) {
			// creating NFSM for <char>+ regular expression
			StateCouple symbol_nfsm = m_1_structure.at(*ch); //accessing NFSM created for <char> reg expression
			expr = *(ch);
			expr += '+';
			m_2_structure.insert(SymbolAndNFSMpairS(expr, make_plus_NFSM(symbol_nfsm.m_init, symbol_nfsm.m_final)));
		}
		else if ((*i).ch == '?' && *ch != ')' && !is_meta_char(*ch)) {
			// creating NFSM for <char>+ regular expression
			StateCouple symbol_nfsm = m_1_structure.at(*ch); //accessing NFSM created for <char> reg expression
			expr = *(ch);
			expr += '?';
			m_2_structure.insert(SymbolAndNFSMpairS(expr, make_question_NFSM(symbol_nfsm.m_init, symbol_nfsm.m_final)));
		}
	}
	const int N_DIGITS = 10;
	TCHAR number[N_DIGITS];
	m_output_ws.append(_T("Done! Number of NFSMs: "));
	swprintf_s(number, N_DIGITS, _T("%d \r\n"), m_2_structure.size());
	m_output_ws.append(number);
	return;
}

void Thompsons::third_stage() {
	bool do_it = true;
	m_output_ws.append(_T(">Third iteration: building NFSMs: <symbol>{metacharacter}|<symbol>{metacheracter} ... "));
	while (do_it) {
		std::string::iterator begin = m_regexpr.begin();
		std::string::iterator end = m_regexpr.end();
		for (std::string::iterator i = begin; i != end; i++) {
			if (i == --m_regexpr.end())
				do_it = false;
			if (*i == '|') {
				std::string::iterator one_before_or = i;
				one_before_or--;
				std::string::iterator one_after_or = i;
				one_after_or++;
				std::string::iterator two_before_or = end;
				std::string::iterator two_after_or = end;
				// check for end of string
				if (one_before_or != m_regexpr.begin()) {
					two_before_or = one_before_or;
					two_before_or--;
				}
				if (one_after_or != --m_regexpr.end()) {
					two_after_or = one_after_or;
					two_after_or++;
				}
				if ((two_before_or != end) && (*one_after_or == '(' || *two_before_or == ')' || *one_after_or == BRACKET_DELIM || *one_before_or == BRACKET_DELIM ||
					(*one_after_or == OR_DELIM && *one_before_or == OR_DELIM))) {
					continue;
				}
				std::string left = "";
				std::string right = "";
				std::string expr = "";
				StateCouple nfsm_left, nfsm_right;

				// <symbol>|<symbol>
				if (!is_meta_char(*one_before_or) && (*one_before_or != OR_DELIM) && !is_meta_char(*one_after_or) &&
					(two_after_or == end || !is_meta_char_nb(*two_after_or)) && *one_after_or != OR_DELIM) {
					char c_left = *one_before_or;
					char c_right = *one_after_or;
					expr += c_left; expr += "|"; expr += c_right;
					nfsm_left = m_1_structure.at(c_left);
					nfsm_right = m_1_structure.at(c_right);

				} // <symbol>metachar|<symbol>
				else if (is_star_plus_quest(*one_before_or) && (*one_before_or != OR_DELIM) && *two_before_or != ')'&&
					!is_meta_char(*one_after_or) && (two_after_or == end || !is_meta_char_nb(*two_after_or)) && *one_after_or != OR_DELIM) {
					left = *two_before_or;
					left += *one_before_or;
					char c_right = *one_after_or;
					expr += left; expr += "|"; expr += c_right;
					nfsm_left = m_2_structure.at(left);
					nfsm_right = m_1_structure.at(c_right);
				}// <symbol>|<symbol>metachar
				else if (!is_meta_char(*one_before_or) && (*one_before_or != OR_DELIM) && *one_before_or != BRACKET_DELIM &&
					(two_after_or == end || is_star_plus_quest(*two_after_or)) && *one_after_or != OR_DELIM) {
					right = *one_after_or;
					right += *two_after_or;
					char c_left = *one_before_or;
					expr += c_left; expr += "|"; expr += right;
					nfsm_left = m_1_structure.at(c_left);
					nfsm_right = m_2_structure.at(right);
				}
				// <symbol>metachar|<symbol>metachar
				else if (is_star_plus_quest(*one_before_or) && (*one_before_or != OR_DELIM) &&
					(two_after_or != end && is_star_plus_quest(*two_after_or)) && *one_after_or != OR_DELIM) {
					right = *one_after_or;
					right += *two_after_or;
					left = *two_before_or;
					left += *one_before_or;
					expr += left; expr += "|"; expr += right;
					nfsm_left = m_2_structure.at(left);
					nfsm_right = m_2_structure.at(right);
				}
				// %number%|<symbol>metachar 
				else if ((*one_before_or == OR_DELIM) && (two_after_or != end && is_star_plus_quest(*two_after_or))
					&& *one_after_or != OR_DELIM) {
					right = *one_after_or;
					right += *two_after_or;
					left = read_subexpr_backwards(one_before_or, OR_DELIM);
					expr += left; expr += "|"; expr += right;
					nfsm_left = m_or_structure.at(left);
					nfsm_right = m_2_structure.at(right);
				}
				// %number%|<symbol>
				else if ((*one_before_or == OR_DELIM) && !is_meta_char(*one_after_or) &&
					(two_after_or == end || !is_meta_char_nb(*two_after_or)) && *one_after_or != OR_DELIM) {
					left = read_subexpr_backwards(one_before_or, OR_DELIM);
					char c_right = *one_after_or;
					expr += left; expr += "|"; expr += c_right;
					nfsm_left = m_or_structure.at(left);
					nfsm_right = m_1_structure.at(c_right);
				} // <symbol>|%number%
				else if (!is_meta_char(*one_before_or) && (*one_before_or != OR_DELIM) && (*one_after_or == OR_DELIM)) {
					right = read_subexpr_forwards(one_after_or, OR_DELIM);
					char c_left = *one_before_or;
					expr += c_left; expr += "|"; expr += right;
					nfsm_left = m_1_structure.at(c_left);
					nfsm_right = m_or_structure.at(right);
				} // <symbol>metachar|%number%
				else if (is_star_plus_quest(*one_before_or) && (*one_before_or != OR_DELIM) && (*one_after_or == OR_DELIM)) {
					right = read_subexpr_forwards(one_after_or, OR_DELIM);
					left = *two_before_or;
					left += *one_before_or;
					expr += left; expr += "|"; expr += right;
					nfsm_left = m_2_structure.at(left);
					nfsm_right = m_or_structure.at(right);
				}
				else {
					//output_ws.append(fourth_it());
					continue;
				}
				StateCouple new_nfsm = make_or_NFSM(nfsm_left.m_init, nfsm_left.m_final, nfsm_right.m_init, nfsm_right.m_final);

				std::string replace = "%" + std::to_string(m_s_id) + "%";
				std::string tmp_regepxr = m_regexpr.replace(m_regexpr.find(expr), expr.length(), replace);
				m_regexpr = tmp_regepxr;
				m_or_structure.insert(SymbolAndNFSMpairS(replace, new_nfsm));
				m_output_ws.append(_T("Subexpression being substituted: "));
				std::wstring ws(expr.begin(), expr.end());
				m_output_ws += ws;
				m_output_ws.append(_T("\r\n "));
				m_output_ws.append(_T("Modified regular expression: "));
				std::wstring ws1(m_regexpr.begin(), m_regexpr.end());
				m_output_ws += ws1;
				m_output_ws.append(_T("\r\n "));
				break; //break for because after string is modifyed by replace iterators are not guarantied to be valid
			}
		}
	}
	const int N_DIGITS = 10;
	TCHAR number[N_DIGITS];
	m_output_ws.append(_T("Done! Number of NFSMs: "));
	swprintf_s(number, N_DIGITS, _T("%d \r\n"), m_or_structure.size());
	m_output_ws.append(number);
	return;
}
void Thompsons::fourth_stage() {
	m_output_ws.append(_T(">Fourth iteration: building NFSMs for brackets... "));
	bool abort = false;
	bool do_only_simple = false;
	while (m_regexpr.find(")") != std::string::npos && !abort) {
		for (std::string::iterator i = m_regexpr.begin(); i != m_regexpr.end(); ++i) {
			std::string::iterator pos = i;
			++pos;
			if (*i == ')') {
				std::string::iterator it = i;
				while (*it != '(') {
					it--;
					if (*it == '|')
						abort = true;
				}
				if (pos == m_regexpr.end())
					do_only_simple = true;
			}
			if (!do_only_simple && (*i == ')') && (i != m_regexpr.end()) && (*pos == '*')) { // (...)* 
				if (!abort)
					make_bracket_NFSM(i, m_output_ws, '*');
				break;
			}
			if (!do_only_simple && (*i == ')') && (i != m_regexpr.end()) && (*pos == '+')) { // (...)+
				if (!abort)
					make_bracket_NFSM(i, m_output_ws, '+');
				break;
			}
			if (!do_only_simple && (*i == ')') && (i != m_regexpr.end()) && (*pos == '?')) { // (...)?
				if (!abort)
					make_bracket_NFSM(i, m_output_ws, '?');
				break;
			}
			if (do_only_simple || (*i == ')') && (i != m_regexpr.end()) && !is_star_plus_quest(*pos)) { // (...)
				if (!abort)
					make_bracket_NFSM(i, m_output_ws, 'S');
				break;
			}
		}
	}
	if (abort) {
		fifth_stage();
		fourth_stage();
	}
	const int N_DIGITS = 10;
	TCHAR number[N_DIGITS];
	m_output_ws.append(_T("Done! Number of NFSMs: "));
	swprintf_s(number, N_DIGITS, _T("%d \r\n"), m_3_structure.size());
	m_output_ws.append(number);
	return;
}
void Thompsons::fifth_stage() {
	int number_of_subcalls = 0;
	m_output_ws.append(_T(">Fifth iteration: building NFSMs: (...){metacharacter}|(...){metacheracter} ... "));
	m_output_ws.append(_T("\r\n "));
	while (m_regexpr.find("|") != std::string::npos) {
		std::string::iterator begin = m_regexpr.begin();
		std::string::iterator end = m_regexpr.end();
		for (std::string::iterator i = begin; i != end; i++) {
			if (*i == '|') {
				std::string::iterator one_before_or = i;
				one_before_or--;
				std::string::iterator one_after_or = i;
				one_after_or++;
				std::string::iterator two_before_or = end;
				std::string::iterator two_after_or = end;
				// check for end of string
				if (one_before_or != m_regexpr.begin()) {
					two_before_or = one_before_or;
					two_before_or--;
				}
				if (one_after_or != --m_regexpr.end()) {
					two_after_or = one_after_or;
					two_after_or++;
				}
				std::string left = "";
				std::string right = "";
				std::string expr = "";
				StateCouple nfsm_left, nfsm_right;

				// ()metachar|<symbol>
				if ((*one_before_or == BRACKET_DELIM) && (*one_after_or != BRACKET_DELIM) && (*one_after_or != OR_DELIM) && (*one_after_or != '(') && (two_after_or == end || !is_meta_char_nb(*two_after_or))) {
					char c_right = *one_after_or;
					left = read_subexpr_backwards(one_before_or, BRACKET_DELIM);
					expr += left; expr += "|"; expr += c_right;
					nfsm_left = m_3_structure.at(left);
					nfsm_right = m_1_structure.at(c_right);
				} // ()metachar|<symbol>metachar
				else if ((*one_before_or == BRACKET_DELIM) && (*one_after_or != BRACKET_DELIM) && (*one_after_or != OR_DELIM) && (*one_after_or != '(') && (two_after_or != end && is_meta_char_nb(*two_after_or))) {
					right = *one_after_or;
					right += *two_after_or;
					left = read_subexpr_backwards(one_before_or, BRACKET_DELIM);
					expr += left; expr += "|"; expr += right;
					nfsm_left = m_3_structure.at(left);
					nfsm_right = m_2_structure.at(right);
				} // ()metachar|()metachar
				else if (*one_before_or == BRACKET_DELIM && *one_after_or == BRACKET_DELIM) {
					left = read_subexpr_backwards(one_before_or, BRACKET_DELIM);
					right = read_subexpr_forwards(one_after_or, BRACKET_DELIM);
					expr += left; expr += "|"; expr += right;
					nfsm_left = m_3_structure.at(left);
					nfsm_right = m_3_structure.at(right);
				} // <symbol>|()metachar
				else if ((*one_before_or != BRACKET_DELIM) && (*one_before_or != OR_DELIM) && (*one_before_or != ')') && !is_meta_char(*one_before_or) && *one_after_or == BRACKET_DELIM) {
					char c_left = *one_before_or;
					right = read_subexpr_forwards(one_after_or, BRACKET_DELIM);
					expr += c_left; expr += "|"; expr += right;
					nfsm_left = m_1_structure.at(c_left);
					nfsm_right = m_3_structure.at(right);
				} // <symbol>metachar|()metachar
				else if ((*one_before_or != BRACKET_DELIM) && (*one_before_or != OR_DELIM) && (*one_before_or != ')') && is_meta_char(*one_before_or) && *one_after_or == BRACKET_DELIM) {
					left = *two_before_or;
					left += *one_before_or;
					right = read_subexpr_forwards(one_after_or, BRACKET_DELIM);
					expr += left; expr += "|"; expr += right;
					nfsm_left = m_2_structure.at(left);
					nfsm_right = m_3_structure.at(right);
				}
				// %number%|()metachar
				else if ((*one_before_or == OR_DELIM) && *one_after_or == BRACKET_DELIM) {
					left = read_subexpr_backwards(one_before_or, OR_DELIM);
					right = read_subexpr_forwards(one_after_or, BRACKET_DELIM);
					expr += left; expr += "|"; expr += right;
					nfsm_left = m_or_structure.at(left);
					nfsm_right = m_3_structure.at(right);
				}// ()metachar|%number%
				else if ((*one_before_or == BRACKET_DELIM) && (*one_after_or == OR_DELIM)) {
					right = read_subexpr_forwards(one_after_or, OR_DELIM);
					left = read_subexpr_backwards(one_before_or, BRACKET_DELIM);
					expr += left; expr += "|"; expr += right;
					nfsm_left = m_3_structure.at(left);
					nfsm_right = m_or_structure.at(right);
				} // %number%|%number%
				else if ((*one_before_or == OR_DELIM) && (*one_after_or == OR_DELIM)) {
					right = read_subexpr_forwards(one_after_or, OR_DELIM);
					left = read_subexpr_backwards(one_before_or, OR_DELIM);
					expr += left; expr += "|"; expr += right;
					nfsm_left = m_or_structure.at(left);
					nfsm_right = m_or_structure.at(right);
				}
				else if ((*one_after_or == '(' || *one_before_or == ')') && number_of_subcalls < 100) {
					number_of_subcalls++;
					continue;
				}
				else if (number_of_subcalls < 100) {
					number_of_subcalls++;
					third_stage();
					fourth_stage();
					third_stage();
					break;
				}
				else {
					fourth_stage();
					number_of_subcalls = 0;
					break;
				}
				StateCouple new_nfsm = make_or_NFSM(nfsm_left.m_init, nfsm_left.m_final, nfsm_right.m_init, nfsm_right.m_final);

				std::string replace = "%" + std::to_string(m_s_id) + "%";
				std::string tmp_regepxr = m_regexpr.replace(m_regexpr.find(expr), expr.length(), replace);
				m_regexpr = tmp_regepxr;
				m_or_structure.insert(SymbolAndNFSMpairS(replace, new_nfsm));
				m_output_ws.append(_T("Subexpression being substituted: "));
				std::wstring ws(expr.begin(), expr.end());
				m_output_ws += ws;
				m_output_ws.append(_T("\r\n "));
				m_output_ws.append(_T("Modified regular expression: "));
				std::wstring ws1(m_regexpr.begin(), m_regexpr.end());
				m_output_ws += ws1;
				m_output_ws.append(_T("\r\n "));
				break;
			}
		}
	}
	return;
}
void Thompsons::concatenate() {
	m_output_ws.append(_T(">Concatenation... "));
	bool first_run = true;
	StateCouple new_nfsm;
	StateCouple sub_nfsm_1;
	StateCouple sub_nfsm_2;
	std::string used_nfsms = "";
	std::string::iterator begin = m_regexpr.begin();
	std::string::iterator end = m_regexpr.end();
	bool end_of_s = false;
	for (std::string::iterator i = begin; i != end; ++i) {

		std::string::iterator pos = i;
		if (i == m_regexpr.end() || ++pos == m_regexpr.end())
			end_of_s = true;

		std::string tmp = "";
		if (*i == OR_DELIM && first_run == true) {
			tmp += read_subexpr_forwards(i , OR_DELIM);
			sub_nfsm_1 = copy_nfsm(m_or_structure.at(tmp).m_init, m_or_structure.at(tmp).m_final);
			if (i == end)
				break;
		}
		else if (*i == BRACKET_DELIM && first_run == true) {
			tmp += read_subexpr_forwards(i, BRACKET_DELIM);
			sub_nfsm_1 = copy_nfsm(m_3_structure.at(tmp).m_init, m_3_structure.at(tmp).m_final);
			if (i == end)
				break;
		}
		else if (!end_of_s && is_meta_char(*pos) && first_run == true) {
			tmp += *i;
			tmp += *pos;
			i = ++pos;
			sub_nfsm_1 = copy_nfsm(m_2_structure.at(tmp).m_init, m_2_structure.at(tmp).m_final);
		}
		else if (first_run == true && pos != m_regexpr.end()) {
			tmp += *i;
			sub_nfsm_1 = copy_nfsm(m_1_structure.at(*i).m_init, m_1_structure.at(*i).m_final);
			++i;
		}
		if (i == m_regexpr.end() || pos == m_regexpr.end())
			end_of_s = true;
		else if (first_run)
			++pos;
		if (pos == m_regexpr.end())
			end_of_s = true;
		first_run = false;
		tmp = "";
		if (!end_of_s && *i == OR_DELIM) {
			tmp += read_subexpr_forwards(i, OR_DELIM);
			i--;
			sub_nfsm_2 = copy_nfsm(m_or_structure.at(tmp).m_init, m_or_structure.at(tmp).m_final);
		}
		else if (!end_of_s && *i == BRACKET_DELIM) {
			tmp += read_subexpr_forwards(i, BRACKET_DELIM);
			i--;
			sub_nfsm_2 = copy_nfsm(m_3_structure.at(tmp).m_init, m_3_structure.at(tmp).m_final);
		}
		else if (!end_of_s && is_meta_char(*pos)) {
			tmp += *i;
			tmp += *pos;
			sub_nfsm_2 = copy_nfsm(m_2_structure.at(tmp).m_init, m_2_structure.at(tmp).m_final);
			i++;
		}
		else if (/*!end_of_s &&*/i != m_regexpr.end() && !is_meta_char(*i)) {
			tmp += *i;
			sub_nfsm_2 = copy_nfsm(m_1_structure.at(*i).m_init, m_1_structure.at(*i).m_final);
		}
		else
			break;
		sub_nfsm_1 = connect_NFSM(sub_nfsm_1.m_init, sub_nfsm_1.m_final, sub_nfsm_2.m_init, sub_nfsm_2.m_final);
	}
	m_4_structure.insert(SymbolAndNFSMpairS(m_regexpr, sub_nfsm_1));
}
StateCouple Thompsons::transform() {
	first_stage();
	second_stage();
	third_stage();
	fourth_stage();
	fifth_stage();
	concatenate();
	return m_4_structure.at(m_regexpr);
}

int NFSM::construct(TransformAlgorithm& algorithm) {
	std::wstring output_ws = read_output_wnd(m_output);
	output_ws.append(_T("Starting Transformation... \r\n"));
		
	m_nfsm = algorithm.transform();
	m_current = m_nfsm.m_init;
	m_states = algorithm.get_states();
	m_s_id = algorithm.get_number_of_states();
	output_ws.append(algorithm.get_log());
	
	output_ws.append(_T("Done... \r\n "));
	m_output->SetWindowTextW(output_ws.c_str());
	m_constructed = true;
	return 0;
}
StateCouple Thompsons::make_or_NFSM(State * s_init_1, State * s_final_1, State * s_init_2, State * s_final_2) {
	State* states = m_states.get();
	//copy nfsms
	StateCouple left = copy_nfsm(s_init_1, s_final_1);
	StateCouple right = copy_nfsm(s_init_2, s_final_2);
	// create new init and final states
	states[m_s_id] = State(m_s_id);
	State * p_a = &(states[m_s_id]);
	m_s_id++;
	states[m_s_id] = State(m_s_id);
	State * p_b = &(states[m_s_id]);
	m_s_id++;

	p_a->set_initial(true);
	p_b->set_final(true);

	left.m_init->set_initial(false);
	left.m_final->set_final(false);
	right.m_init->set_initial(false);
	right.m_final->set_final(false);

	p_a->set_transition(m_t_id++, LAMBDA_CH, left.m_init);
	p_a->set_transition(m_t_id++, LAMBDA_CH, right.m_init);
	left.m_final->set_transition(m_t_id++, LAMBDA_CH, p_b);
	right.m_final->set_transition(m_t_id++, LAMBDA_CH, p_b);

	return StateCouple(p_a, p_b);
}

StateCouple Thompsons::copy_nfsm(State * init, State * final_s) {
	//first copy initial state
	State* states = m_states.get();
	states[m_s_id] = *init;
	init->set_empty(true);
	states[m_s_id].set_id(m_s_id);
	m_s_id++;
	// then copy final state
	states[m_s_id] = *final_s;
	final_s->set_empty(true);
	states[m_s_id].set_id(m_s_id);
	states[m_s_id].set_final(true);
	int final_id = m_s_id;
	StateCouple copy = StateCouple(&states[m_s_id - 1], &states[m_s_id]);
	m_s_id++;

	// now copy states in between
	int done = 0;
	int done_check = 0;
	std::vector<State *> current;
	std::vector<State *> current_new;
	std::map<int, int> book; // first int is id of copied state and second int is id of the copy
	current.push_back(&states[m_s_id - 2]);
	while (1) {
		done_check = done;
		for (std::vector<State *>::iterator j = current.begin(); j != current.end(); j++) {
			for (std::vector<Transition>::iterator i = (*j)->get_out().begin(); i != (*j)->get_out().end(); i++) {
				if (i->get_to() == final_s) {
					i->set_to(states[final_id]);
					//current_new.push_back(&m_states[m_s_id]);
					done++;
				}
				else if(i->get_to() != NULL) {
					if (book.find(i->get_to()->get_id()) != book.end()) {
						State & from = states[book.at(i->get_to()->get_id())];
						i->set_to(from);
						from.get_in().push_back(Transition(i->get_id(), i->get_symbol(), *j, &from));
					}
					else {
						states[m_s_id] = *(i->get_to()); // copy state
						i->get_to()->set_empty(true);
						// book keeping
						book[i->get_to()->get_id()] = m_s_id;

						states[m_s_id].set_id(m_s_id);
						i->set_to(states[m_s_id]); // make transition to the newly copied state
						states[m_s_id].get_in().clear();
						states[m_s_id].get_in().push_back(Transition(i->get_id(), i->get_symbol(), *j, &states[m_s_id]));
						current_new.push_back(&states[m_s_id]);
						m_s_id++;
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
	return copy;
}

int RUN::formal(int length) { //compute bounds on number of NFSM copies
	int max_n = 0; // maximum number of copies
	std::vector<std::vector<NFSM>> runs;
	std::set<char> set_of_symbols;
	std::set<int> visited_states;
	m_nfsms.clear(); //clear after posible make_transition call
	m_nfsms.push_back(*m_nfsm);
	m_nfsms.reserve(MAX_NUMBER_OF_NFSM_COPIES);
	runs.reserve(200);
	runs.push_back(m_nfsms);
	std::vector<std::vector<NFSM>> tmp_vv_NFSM;
	int n_invalidated = 0;
	while (runs.size() > 0) {
		
		for (std::vector<std::vector<NFSM>>::iterator tt = runs.begin(); tt != runs.end(); tt++) {

			run_lambda(NULL, &(*tt), false, &visited_states, &n_invalidated); // first stage: perform all possible §-transitions
			// lambda-transitions are creators of the copies
			//actually delete nfsms 
			while (n_invalidated-- > 0)
				for (std::vector<NFSM>::iterator j = tt->begin(); j != tt->end(); j++) {
					if (!j->is_valid()) {
						tt->erase(j);
						break;
					}
				}
			n_invalidated = 0;

			if (max_n < tt->size())
				max_n = tt->size(); // update max // record max number of states reached
		}
		std::vector<NFSM> tmp;
		for (std::vector<std::vector<NFSM>>::iterator tt = runs.begin(); tt != runs.end(); tt++) {
			tmp.insert(tmp.end(), tt->begin(), tt->end());
		}
				// create a set of symbols of all transitions of current states reached
				set_of_symbols.clear(); // each time a new set of symbols
				for (std::vector<NFSM>::iterator it = tmp.begin(); it != tmp.end(); it++) {
					for (std::vector<Transition>::iterator tr = it->get_current()->get_out().begin(); tr != it->get_current()->get_out().end(); tr++) {
						if (tr->get_symbol() != LAMBDA_CH)
							set_of_symbols.insert(tr->get_symbol());
					}
				}
			
				for (std::set<char>::iterator it = set_of_symbols.begin(); it != set_of_symbols.end(); it++) {
					//create NFSM vector for each symbol, merge NFSM from other NFSM vectors
					std::vector<NFSM> tmp_2 = tmp;
					// normal transitions are destructors of copies
					for (std::vector<NFSM>::iterator j = tmp_2.begin(); j != tmp_2.end(); j++) {
						transition_for_symbol(&tmp_2, &(*j), *it, false, &n_invalidated, true);
					}
					//actually delete nfsms 
					while (n_invalidated-- > 0)
						for (std::vector<NFSM>::iterator j = tmp_2.begin(); j != tmp_2.end(); j++) {
							if (!j->is_valid()) {
								tmp_2.erase(j);
								break;
							}
						}
					n_invalidated = 0;
					tmp_vv_NFSM.push_back(tmp_2);
				}
			
		runs = tmp_vv_NFSM;
		tmp_vv_NFSM.clear();

	}
	if (!max_n)
		max_n = 1;
	return max_n;
}

TransType RUN::make_transition()// for empty string
{
	if (run_lambda(this, NULL, true, NULL, NULL) == TransType::FINAL_STATE_LAMBDA)
		return TransType::FINAL_STATE_LAMBDA;
	else
		return TransType::NOT_FINAL_LAMBDA;
}
TransType RUN::make_transition(char input, bool last_ch)
{
	std::wstring output_ws = read_output_wnd(m_output);
	const int N_SIZE = 90;
	
	if (run_lambda(this, NULL, false, NULL, NULL) == TransType::FINAL_STATE_LAMBDA && last_ch)
		return TransType::FINAL_STATE_LAMBDA;
	
	// transitions for other symbols
	TransType t_type = TransType::NOT_FINAL;
	int n_invalidated = 0;
	for (std::vector<NFSM>::iterator j = m_nfsms.begin(); j != m_nfsms.end(); j++) {
		t_type = transition_for_symbol(NULL, &(*j), input, last_ch, &n_invalidated);
	}
	TCHAR number[N_SIZE];
	swprintf_s(number, 90, _T("runtime number of NFSM copies: %d \r\n"), m_nfsms.size());
	output_ws.append(number);
	//actually delete nfsms 
	while (n_invalidated-- != 0)
		for (std::vector<NFSM>::iterator j = m_nfsms.begin(); j != m_nfsms.end(); j++) {
			if (!j->is_valid()) {
				m_nfsms.erase(j);
				break;
			}
		}

	if (t_type == TransType::INVALID_TRANSITION && last_ch && (m_nfsms.size() == 0))
		return t_type;

	m_output->SetWindowTextW(output_ws.c_str());

	if (run_lambda(this, NULL, last_ch, NULL, NULL) == TransType::FINAL_STATE_LAMBDA && last_ch)
		return TransType::FINAL_STATE_LAMBDA;

	return t_type;
}

TransType run_lambda(RUN * obj, std::vector<NFSM> * p_nfsms, bool last_ch, std::set<int> * visited_s = NULL, int * n_invalidated = NULL) {
	TransType t_type;
	int n_invalidated_2 = 0;
	int max_t = 20;
	std::set<int> visited;
	std::set<int>::iterator cur;
	std::set<int>::iterator no_rep;
	std::vector<NFSM> * nfsm = NULL;
	if (obj != NULL) 
		nfsm = &(obj->m_nfsms);
	else if (p_nfsms != NULL)
		nfsm = p_nfsms;

	if (nfsm == NULL)
		return TransType::ERROR_;
	
	std::vector<NFSM> NFSMs_tmp;
	bool lambda = false;
	do {
		lambda = false;
		for (auto j = nfsm->begin(); j != nfsm->end(); j++) {
			int n_of_transitions = 0;
			auto T_begin = j->get_current()->get_out().begin();
			auto T_end = j->get_current()->get_out().end();
			// only lambda transitions
			for (auto i = T_begin; i != T_end; i++) {
				cur = visited.find(i->get_to()->get_id());
				if (visited_s)
					no_rep = visited_s->find(i->get_to()->get_id()); // for loops prevention
				if (i->get_symbol() == LAMBDA_CH && cur == visited.end() && (visited_s == NULL || no_rep == visited_s->end())) {
					lambda = true;
					if (n_of_transitions > 0) { // nondeterministic transition
						NFSM tmp = *j;
						visited.insert(i->get_to()->get_id());
						if (visited_s)
							visited_s->insert(j->get_current()->get_id()); // for loops prevention
						tmp.set_current(i->get_to());
						NFSMs_tmp.push_back(tmp);
						n_of_transitions++;
						if (i->get_to()->is_final() && last_ch)
							return TransType::FINAL_STATE_LAMBDA;
						
						t_type = TransType::NOT_FINAL_LAMBDA;
					}
					else if (n_of_transitions == 0 && (visited_s == NULL || no_rep == visited_s->end())) { // deterministictransition
						visited.insert(i->get_to()->get_id());
						if (visited_s)
							visited_s->insert(j->get_current()->get_id());// for loops prevention
						j->set_current(i->get_to());
						n_of_transitions++;
						if (i->get_to()->is_final() && last_ch)
							return TransType::FINAL_STATE_LAMBDA;
						t_type = TransType::NOT_FINAL_LAMBDA;
					}
				}
				else if (n_invalidated != NULL && visited_s != NULL && 
					((i->get_to()->is_final() && i->get_symbol() == LAMBDA_CH) || no_rep != visited_s->end())) {
					(*n_invalidated) += 1;
					j->set_invalid();
				}
			}
		}
		if (NFSMs_tmp.size() > 0) 
			nfsm->insert(nfsm->end(), NFSMs_tmp.begin(), NFSMs_tmp.end());
		
		NFSMs_tmp.clear();
	} 
	while (lambda && (--max_t > 0));

	for (std::vector<NFSM>::iterator j = nfsm->begin(); j != nfsm->end(); j++) {
		if (j->get_current()->is_final() && !last_ch) {
			j->set_invalid();
			n_invalidated_2++;
		}
	}
	//actually delete nfsms 
	while (n_invalidated_2-- > 0)
		for (std::vector<NFSM>::iterator j = nfsm->begin(); j != nfsm->end(); j++) {
			if (!j->is_valid() && j->get_current()->is_final()) {
				nfsm->erase(j);
				break;
			}
		}
	return TransType::NOT_FINAL_LAMBDA;
}

bool is_meta_char(char ch) {
	for (std::vector<char>::iterator j = METACHAR_.begin(); j != METACHAR_.end(); ++j)
		if (ch == *j)
			return true;
	return false;
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

StateCouple Thompsons::make_star_NFSM(State * s_init, State * s_final) {
	State* states = m_states.get();
	//copy nfsm
	StateCouple copy = copy_nfsm(s_init, s_final);
	// create new init and final states
	states[m_s_id] = State(m_s_id);
	State * p_a = &(states[m_s_id]);
	m_s_id++;
	states[m_s_id] = State(m_s_id);
	State * p_b = &(states[m_s_id]);
	m_s_id++;

	p_a->set_initial(true);
	p_b->set_final(true);

	copy.m_init->set_initial(false);
	copy.m_final->set_final(false);

	p_a->set_transition(m_t_id++, LAMBDA_CH, copy.m_init);
	p_a->set_transition(m_t_id++, LAMBDA_CH, p_b);
	copy.m_final->set_transition(m_t_id++, LAMBDA_CH, p_b);
	copy.m_final->set_transition(m_t_id++, LAMBDA_CH, copy.m_init);

	return StateCouple(p_a, p_b);
}
StateCouple Thompsons::make_plus_NFSM(State * s_init, State * s_final) {
	State* states = m_states.get();
	//copy nfsm
	StateCouple copy = copy_nfsm(s_init, s_final);
	// create new init and final states
	states[m_s_id] = State(m_s_id);
	State * p_a = &(states[m_s_id]);
	m_s_id++;
	states[m_s_id] = State(m_s_id);
	State * p_b = &(states[m_s_id]);
	m_s_id++;

	p_a->set_initial(true);
	p_b->set_final(true);

	copy.m_init->set_initial(false);
	copy.m_final->set_final(false);

	p_a->set_transition(m_t_id++, LAMBDA_CH, copy.m_init);
	copy.m_final->set_transition(m_t_id++, LAMBDA_CH, p_b);
	copy.m_final->set_transition(m_t_id++, LAMBDA_CH, copy.m_init);
	return StateCouple(p_a, p_b);
}
StateCouple Thompsons::make_simple_NFSM(State * s_init, State * s_final) {
	//copy nfsm
	StateCouple copy = copy_nfsm(s_init, s_final);
	return copy;
}
StateCouple Thompsons::make_question_NFSM(State * s_init, State * s_final) {
	State* states = m_states.get();
	//copy nfsm
	StateCouple copy = copy_nfsm(s_init, s_final);
	// create new init and final states
	states[m_s_id] = State(m_s_id);
	State * p_a = &(states[m_s_id]);
	m_s_id++;
	states[m_s_id] = State(m_s_id);
	State * p_b = &(states[m_s_id]);
	m_s_id++;

	p_a->set_initial(true);
	p_b->set_final(true);

	copy.m_init->set_initial(false);
	copy.m_final->set_final(false);

	p_a->set_transition(m_t_id++, LAMBDA_CH, copy.m_init);
	p_a->set_transition(m_t_id++, LAMBDA_CH, p_b);
	copy.m_final->set_transition(m_t_id++, LAMBDA_CH, p_b);
	return StateCouple(p_a, p_b);
}
StateCouple Thompsons::make_one_symbol_NFSM(char ch) {
	State* states = m_states.get();
	states[m_s_id] = State(m_s_id);
	State * p_a = &(states[m_s_id]);
	m_s_id++;
	states[m_s_id] = State(m_s_id);
	State * p_b = &(states[m_s_id]);
	m_s_id++;
	p_a->set_initial(true);
	p_b->set_final(true);
	// handle "." metachar
	if (ch == '.')
		p_a->set_transition(m_t_id++, BETA_CH, p_b);
	else
		p_a->set_transition(m_t_id++, ch, p_b);
	return StateCouple(p_a, p_b);
}
StateCouple Thompsons::connect_NFSM(State * s_init_1, State * s_final_1, State * s_init_2, State * s_final_2) {
	s_final_1->set_final(false);
	s_final_1->set_transition(m_t_id++, LAMBDA_CH, s_init_2);
	s_init_2->set_initial(false);
	s_final_2->set_final(true);
	return StateCouple(s_init_1, s_final_2);
}

StateCouple Thompsons::make_bracket_NFSM(std::string::iterator i, std::wstring & output_ws, char type) {
	std::string sub_expr = "";
	std::string tmp = "";
	
	StateCouple new_nfsm;
	StateCouple sub_nfsm_1;
	StateCouple sub_nfsm_2;
	bool first_iter = true;

	std::string::iterator prev_pos;
	for (std::string::iterator j = i - 1; j != m_regexpr.begin(); --j) {
		if (*j != '(' && first_iter == true) {
			first_iter = false;
			prev_pos = j;
			--prev_pos;
			if (is_meta_char(*j) && prev_pos != m_regexpr.begin()) {
				tmp = *prev_pos;
				tmp += *j;
				sub_expr.insert(sub_expr.begin(), *j);
				sub_expr.insert(sub_expr.begin(), *prev_pos);
				sub_nfsm_1 = copy_nfsm(m_2_structure.at(tmp).m_init, m_2_structure.at(tmp).m_final);
				j = --prev_pos;
			}
			else if (*j == OR_DELIM && is_numeric(*(--j))) {
				std::string tmp_2 = read_subexpr_backwards(++j, OR_DELIM);
				sub_nfsm_1 = copy_nfsm(m_or_structure.at(tmp_2).m_init, m_or_structure.at(tmp_2).m_final);
				sub_expr = tmp_2 + sub_expr;
			}
			else if (*j == BRACKET_DELIM && is_numeric(*(--j))) {
				std::string tmp_2 = read_subexpr_backwards(++j, BRACKET_DELIM);
				sub_nfsm_1 = copy_nfsm(m_3_structure.at(tmp_2).m_init, m_3_structure.at(tmp_2).m_final);
				sub_expr = tmp_2 + sub_expr;
			}
			else if (!is_meta_char(*j)){
				sub_expr.insert(sub_expr.begin(), *j);
				sub_nfsm_1 = copy_nfsm(m_1_structure.at(*j).m_init, m_1_structure.at(*j).m_final);
				j = prev_pos;
			}
			first_iter == false;
		} 
		tmp = "";
		if (*j != '(' && j != m_regexpr.begin()) {
			prev_pos = j;
			--prev_pos;
			if (is_meta_char(*j) && prev_pos != m_regexpr.begin()) {
				tmp += *prev_pos;
				tmp += *j;
				sub_expr.insert(sub_expr.begin(), *j);
				sub_expr.insert(sub_expr.begin(), *prev_pos);
				sub_nfsm_2 = copy_nfsm(m_2_structure.at(tmp).m_init, m_2_structure.at(tmp).m_final);
				j = prev_pos;
			}
			else if (*j == OR_DELIM && is_numeric(*(--j))) {
				std::string tmp_2 = read_subexpr_backwards(++j, OR_DELIM);
				j++;
				sub_nfsm_2 = copy_nfsm(m_or_structure.at(tmp_2).m_init, m_or_structure.at(tmp_2).m_final);
				sub_expr = tmp_2 + sub_expr;
			}
			else if (*j == BRACKET_DELIM && is_numeric(*(--j))) {
				std::string tmp_2 = read_subexpr_backwards(++j, BRACKET_DELIM);
				j++;
				sub_nfsm_2 = copy_nfsm(m_3_structure.at(tmp_2).m_init, m_3_structure.at(tmp_2).m_final);
				sub_expr = tmp_2 + sub_expr;
			}
			else if(!is_meta_char(*j)) {
				sub_expr.insert(sub_expr.begin(), *j);
				sub_nfsm_2 = copy_nfsm(m_1_structure.at(*j).m_init, m_1_structure.at(*j).m_final);
			}
		}
		else
			break;
		sub_nfsm_1 = connect_NFSM(sub_nfsm_2.m_init, sub_nfsm_2.m_final, sub_nfsm_1.m_init, sub_nfsm_1.m_final);
	}
	sub_expr.insert(sub_expr.begin(), '(');
	output_ws.append(_T("Regular sub-expression being substituted: "));
	
	if (type == '*') {
		new_nfsm = make_star_NFSM(sub_nfsm_1.m_init, sub_nfsm_1.m_final);
		sub_expr += ")*";
	}
	else if (type == '+') {
		new_nfsm = make_plus_NFSM(sub_nfsm_1.m_init, sub_nfsm_1.m_final);
		sub_expr += ")+";
	}
	else if (type == '?') {
		new_nfsm = make_question_NFSM(sub_nfsm_1.m_init, sub_nfsm_1.m_final);
		sub_expr += ")?";
	}
	else if (type == 'S') {// simple brackets without metachar
		new_nfsm = make_simple_NFSM(sub_nfsm_1.m_init, sub_nfsm_1.m_final);
		sub_expr += ")";
	}
	
	std::string replace = "$" + std::to_string(m_s_id) + "$";
	std::string tmp_regepxr = m_regexpr.replace(m_regexpr.find(sub_expr), sub_expr.length(), replace);
	m_regexpr = tmp_regepxr;
	m_3_structure.insert(SymbolAndNFSMpairS(replace, new_nfsm));
	std::wstring ws(sub_expr.begin(), sub_expr.end());
	output_ws += ws;
	output_ws.append(_T("\r\n "));
	output_ws.append(_T("Modified regular expression: "));
	std::wstring ws1(m_regexpr.begin(), m_regexpr.end());
	output_ws += ws1;
	output_ws.append(_T("\r\n "));
	return new_nfsm;

}

void NFSM::optimize() {
	State* states = m_states.get();
	int start_id = m_nfsm.m_init->get_id();

	int num = 5;
	while (num--) {
		for (int i = start_id; i <= m_s_id; i++) {
			State * st = &states[i];
			bool only_lambda = true;
			if (st->is_empty() == true)
				continue;


			if (st->get_in().size() == 1 && (st->get_in().at(0).get_symbol() == LAMBDA_CH)) { // just one input to the state
				for (auto it = st->get_out().begin(); it != st->get_out().end(); it++)
					if (it->get_symbol() != LAMBDA_CH) // outputs only lambda
						only_lambda = false;
			}
			else
				continue;
			if (only_lambda) { // state is not needed, delete it
				State * from = st->get_in().at(0).get_from();
				//delete transition to the superflous state
				for (auto it = from->get_out().begin(); it != from->get_out().end(); it++) {
					if (it->get_to() == st) {
						from->get_out().erase(it);
						break;
					}
				}
				// assign the outputs of the sperflous state to the previous state
				for (auto it = st->get_out().begin(); it != st->get_out().end(); it++) {
					for (auto j = it->get_to()->get_in().begin(); j != it->get_to()->get_in().end(); j++) {
						if (j->get_from() == st)
							j->set_from(*from);
					}
					from->get_out().push_back(*it);
				}
				// delete the state
				states[i].set_empty(true);
			}
		}
	}
	//start with many lambda-inputs, one output
	num = 5;
	while (num--) {
		for (int i = start_id; i <= m_s_id; i++) {
			State * st = &states[i];
			bool only_lambda = true;
			if (st->is_empty() == true)
				continue;


			if (st->get_out().size() == 1 && (st->get_out().at(0).get_symbol() == LAMBDA_CH)) { // just one input to the state
				for (auto it = st->get_in().begin(); it != st->get_in().end(); it++)
					if (it->get_symbol() != LAMBDA_CH) // outputs only lambda
						only_lambda = false;
			}
			else
				continue;
			if (only_lambda) { // state is not needed, delete it
				State * to = st->get_out().at(0).get_to();
				//delete transition from the superflous state
				for (auto it = to->get_out().begin(); it != to->get_out().end(); it++) {
					if (it->get_from() == st) {
						to->get_in().erase(it);
						break;
					}
				}
				// assign the inputs of the sperflous state to the next state
				for (auto it = st->get_in().begin(); it != st->get_in().end(); it++) {
					for (auto j = it->get_from()->get_out().begin(); j != it->get_from()->get_out().end(); j++) {
						if (j->get_to() == st)
							j->set_to(*to);
					}
					to->get_in().push_back(*it);
				}
				// delete the state
				states[i].set_empty(true);

			}
		}
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
void NFSM::write_nfsm(std::string file_name) {
	// open a file in write mode.
	std::ofstream out_file;
	std::string graph;
	std::string graph_tmp;
	out_file.open(file_name);
	// header
	out_file << "strict digraph {" << std::endl;
	out_file << "	rankdir = LR" << std::endl;

	State * start = m_nfsm.m_init;
	std::vector<State *> current_s;
	current_s.push_back(start);
	std::vector<State *> current_s_new;
	int size = m_s_id/2;
	while (--size > 0) {
		for (std::vector<State *>::iterator j = current_s.begin(); j != current_s.end(); j++) {
			for (std::vector<Transition>::iterator i = (*j)->get_out().begin(); i != (*j)->get_out().end(); i++) {
				graph_tmp = "	" + std::to_string((*j)->get_id());
				graph_tmp += " -> ";
				if (i->get_to()->is_final())
					graph_tmp += std::to_string(i->get_to()->get_id()) + ".";
				else
					graph_tmp += std::to_string(i->get_to()->get_id());
				graph_tmp += " [label=";
				graph_tmp += '"';
				graph_tmp += i->get_symbol();
				graph_tmp += '"';
				graph_tmp += "]";
				if (graph.find(graph_tmp) == std::string::npos) {
					graph += graph_tmp + "\n";
					if (!i->get_to()->is_final())
						current_s_new.push_back(i->get_to());
				}
			}
		}
		current_s = current_s_new;
		current_s_new.clear();
	}
	out_file << graph << std::endl << "}" <<std::endl;
	out_file.close();
}

bool is_star_plus_quest(char ch) {
	switch (ch) {
	case '*': return true;
	case '+': return true;
	case '?': return true;
	default: return false;
	}
}
TransType RUN::transition_for_symbol(std::vector<NFSM> * p_nfsms, NFSM * nfsm, char input, bool last_ch, int * n_inval, bool formal) {
	int n_of_transitions = 0;
	std::vector<NFSM> NFSMs_tmp;
	TransType t_type = TransType::INVALID_TRANSITION;
	std::vector<Transition>::iterator T_begin = nfsm->get_current()->get_out().begin();
	std::vector<Transition>::iterator T_end = nfsm->get_current()->get_out().end();
	for (std::vector<Transition>::iterator i = T_begin; i != T_end; ++i) {

		if (nfsm->is_valid() && (i->get_symbol() == input || i->get_symbol() == BETA_CH
			/*|| (formal && i->m_symbol != LAMBDA_CH)*/)  && n_of_transitions > 0) {
			NFSM tmp = *nfsm;
			tmp.set_current(i->get_to());
			NFSMs_tmp.push_back(tmp);
			n_of_transitions++;
			if (i->get_to()->is_final() && last_ch)
				return TransType::FINAL_STATE;
			else if (i->get_to()->is_final()) {
				nfsm->set_invalid();
				*(n_inval) += 1;
			}
			t_type = TransType::NOT_FINAL;
		}
		else if (nfsm->is_valid() && (i->get_symbol() == input || i->get_symbol() == BETA_CH
			/*|| (formal && i->m_symbol != LAMBDA_CH)*/) && n_of_transitions == 0) {
			nfsm->set_current(i->get_to());
			n_of_transitions++;
			if (i->get_to()->is_final() && last_ch)
				return TransType::FINAL_STATE;
			else if (i->get_to()->is_final()) {
				nfsm->set_invalid();
				*(n_inval) += 1;
			}
			t_type = TransType::NOT_FINAL;
		}
		else if (nfsm->is_valid()) {
			//delete nfsm
			nfsm->set_invalid();
			*(n_inval) += 1;
			t_type = TransType::INVALID_TRANSITION;
		}
	}
	if (p_nfsms != NULL)
		p_nfsms->insert(p_nfsms->end(), NFSMs_tmp.begin(), NFSMs_tmp.end());
	else
		m_nfsms.insert(m_nfsms.end(), NFSMs_tmp.begin(), NFSMs_tmp.end());
	NFSMs_tmp.clear();
	return t_type;
}
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
	it--;
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
	it++;
	return str;
}