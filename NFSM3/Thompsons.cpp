#include "stdafx.h"
#include "Thompsons.h"
#include "SimpleLogger.h"
#include "Helpers.h"

Thompsons::Thompsons(std::string regexpr) : m_regexpr(regexpr),
m_states{ new State[MAX_NUMBER_OF_STATES],
std::default_delete<State[]>() }, m_logger{ SimpleLogger::GetInstance() }, m_logging{ false }
{
	State* states = m_states.get();
	for (int i = 0; i < MAX_NUMBER_OF_STATES; i++)
		states[i] = State(); //ititialize array with empty states	
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
	if (m_logging) m_logger << m_output_ws;
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
		else
			ch = (i->pos);

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
	if (m_logging) m_logger << m_output_ws;
	return;
}

void Thompsons::third_stage() {
	bool do_it = true;
	m_output_ws.append(_T(">Third iteration: building NFSMs: <symbol>{metacharacter}|<symbol>{metacheracter} ... \r\n"));
	while (do_it) {
		std::string::iterator begin = m_regexpr.begin();
		std::string::iterator end = m_regexpr.end();
		for (std::string::iterator i = begin; i != end; i++) {
			if (i == --m_regexpr.end())
				do_it = false;
			if (*i == '|') {
				std::string::iterator one_before_or = { i }, one_after_or = { i };
				std::string::iterator two_before_or = --one_before_or;
				std::string::iterator two_after_or = ++one_after_or;
				// check for end of string
				if (one_before_or != m_regexpr.begin())
					--two_before_or;

				if (one_after_or != --m_regexpr.end())
					++two_after_or;

				if ((two_before_or != end) && (*one_after_or == '(' || *two_before_or == ')'
					|| *one_after_or == BRACKET_DELIM || *one_before_or == BRACKET_DELIM
					|| (*one_after_or == OR_DELIM && *one_before_or == OR_DELIM))) {
					continue;
				}
				std::string left = "";
				std::string right = "";
				std::string expr = "";
				StateCouple nfsm_left, nfsm_right;

				// <symbol>|<symbol>
				if (!is_meta_char(*one_before_or) && (*one_before_or != OR_DELIM) && !is_meta_char(*one_after_or)
					&& (two_after_or == end || !is_meta_char_nb(*two_after_or)) && *one_after_or != OR_DELIM) {
					char c_left = *one_before_or;
					char c_right = *one_after_or;
					expr += c_left; expr += "|"; expr += c_right;
					nfsm_left = m_1_structure.at(c_left);
					nfsm_right = m_1_structure.at(c_right);

				} // <symbol>metachar|<symbol>
				else if (is_star_plus_quest(*one_before_or) && (*one_before_or != OR_DELIM) && *two_before_or != ')'
					&& !is_meta_char(*one_after_or) && (two_after_or == end || !is_meta_char_nb(*two_after_or))
					&& *one_after_or != OR_DELIM) {
					left = *two_before_or;
					left += *one_before_or;
					char c_right = *one_after_or;
					expr += left; expr += "|"; expr += c_right;
					nfsm_left = m_2_structure.at(left);
					nfsm_right = m_1_structure.at(c_right);
				}// <symbol>|<symbol>metachar
				else if (!is_meta_char(*one_before_or) && (*one_before_or != OR_DELIM) && *one_before_or != BRACKET_DELIM
					&& (two_after_or == end || is_star_plus_quest(*two_after_or)) && *one_after_or != OR_DELIM) {
					right = *one_after_or;
					right += *two_after_or;
					char c_left = *one_before_or;
					expr += c_left; expr += "|"; expr += right;
					nfsm_left = m_1_structure.at(c_left);
					nfsm_right = m_2_structure.at(right);
				}
				// <symbol>metachar|<symbol>metachar
				else if (is_star_plus_quest(*one_before_or) && (*one_before_or != OR_DELIM)
					&& (two_after_or != end && is_star_plus_quest(*two_after_or)) && *one_after_or != OR_DELIM) {
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
				else if ((*one_before_or == OR_DELIM) && !is_meta_char(*one_after_or)
					&& (two_after_or == end || !is_meta_char_nb(*two_after_or)) && *one_after_or != OR_DELIM) {
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
				else if (is_star_plus_quest(*one_before_or) && (*one_before_or != OR_DELIM)
					&& (*one_after_or == OR_DELIM)) {
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
	if (m_logging) m_logger << m_output_ws;
	return;
}
void Thompsons::fourth_stage() {
	m_output_ws.append(_T(">Fourth iteration: building NFSMs for brackets... "));
	bool abort = false, do_only_simple = false;
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
	if (m_logging) m_logger << m_output_ws;
	return;
}
void Thompsons::fifth_stage() {
	int number_of_subcalls = 0;
	m_output_ws.append(_T(">Fifth iteration: building NFSMs: (...){metacharacter}|(...){metacheracter} ... \r\n"));
	while (m_regexpr.find("|") != std::string::npos) {
		std::string::iterator begin = { m_regexpr.begin() }, end = { m_regexpr.end() };
		for (std::string::iterator i = begin; i != end; i++) {
			if (*i == '|') {
				std::string::iterator one_before_or = { i }, one_after_or = { i };
				std::string::iterator two_before_or = --one_before_or;
				std::string::iterator two_after_or = ++one_after_or;
				// check for end of string
				if (one_before_or != m_regexpr.begin())
					--two_before_or;

				if (one_after_or != --m_regexpr.end())
					++two_after_or;

				std::string left, right, expr = { "" };
				StateCouple nfsm_left, nfsm_right;

				// ()metachar|<symbol>
				if ((*one_before_or == BRACKET_DELIM) && (*one_after_or != BRACKET_DELIM)
					&& (*one_after_or != OR_DELIM) && (*one_after_or != '(')
					&& (two_after_or == end || !is_meta_char_nb(*two_after_or))) {
					char c_right = *one_after_or;
					left = read_subexpr_backwards(one_before_or, BRACKET_DELIM);
					expr += left; expr += "|"; expr += c_right;
					nfsm_left = m_3_structure.at(left);
					nfsm_right = m_1_structure.at(c_right);
				} // ()metachar|<symbol>metachar
				else if ((*one_before_or == BRACKET_DELIM) && (*one_after_or != BRACKET_DELIM)
					&& (*one_after_or != OR_DELIM) && (*one_after_or != '(')
					&& (two_after_or != end && is_meta_char_nb(*two_after_or))) {
					right = *one_after_or;
					right += *two_after_or;
					left = read_subexpr_backwards(one_before_or, BRACKET_DELIM);
					expr += left; expr += "|"; expr += right;
					nfsm_left = m_3_structure.at(left);
					nfsm_right = m_2_structure.at(right);
				} // ()metachar|()metachar
				else if (*one_before_or == BRACKET_DELIM && *one_after_or == BRACKET_DELIM) {
					left = read_subexpr_backwards(one_before_or, BRACKET_DELIM);
					right = read_subexpr_forwards(one_after_or++, BRACKET_DELIM);
					expr += left; expr += "|"; expr += right;
					nfsm_left = m_3_structure.at(left);
					nfsm_right = m_3_structure.at(right);
				} // <symbol>|()metachar
				else if ((*one_before_or != BRACKET_DELIM) && (*one_before_or != OR_DELIM)
					&& (*one_before_or != ')') && !is_meta_char(*one_before_or) && *one_after_or == BRACKET_DELIM) {
					char c_left = *one_before_or;
					right = read_subexpr_forwards(one_after_or, BRACKET_DELIM);
					expr += c_left; expr += "|"; expr += right;
					nfsm_left = m_1_structure.at(c_left);
					nfsm_right = m_3_structure.at(right);
				} // <symbol>metachar|()metachar
				else if ((*one_before_or != BRACKET_DELIM) && (*one_before_or != OR_DELIM)
					&& (*one_before_or != ')') && is_meta_char(*one_before_or) && *one_after_or == BRACKET_DELIM) {
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
					third_stage(); fourth_stage(); third_stage();
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
	if (m_logging) m_logger << m_output_ws;
	return;
}
void Thompsons::concatenate() {
	m_output_ws.append(_T(">Concatenation... "));
	bool first_run = true;
	StateCouple new_nfsm{ nullptr, nullptr };
	StateCouple sub_nfsm_1{ nullptr, nullptr };
	StateCouple sub_nfsm_2{ nullptr, nullptr };
	std::string used_nfsms = "";
	std::string::iterator begin = m_regexpr.begin();
	std::string::iterator end = m_regexpr.end();
	bool end_of_s = false;
	if (m_1_structure.size() > 1) {
		for (auto i = begin; i != end; ++i) {

			std::string::iterator pos = i;
			if (i == m_regexpr.end() || ++pos == m_regexpr.end())
				end_of_s = true;

			std::string tmp = "";
			if (*i == OR_DELIM && first_run == true) {
				tmp += read_subexpr_forwards(i, OR_DELIM);
				i++;
				sub_nfsm_1 = copy_nfsm(m_or_structure.at(tmp).m_init, m_or_structure.at(tmp).m_final);
				if (i == end)
					break;
			}
			else if (*i == BRACKET_DELIM && first_run == true) {
				tmp += read_subexpr_forwards(i, BRACKET_DELIM);
				i++;
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
				sub_nfsm_2 = copy_nfsm(m_or_structure.at(tmp).m_init, m_or_structure.at(tmp).m_final);
			}
			else if (!end_of_s && *i == BRACKET_DELIM) {
				tmp += read_subexpr_forwards(i, BRACKET_DELIM);
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
			if (sub_nfsm_2.m_init == nullptr || sub_nfsm_1.m_init == nullptr) throw Nullptr("In concatenation phase one of the two NFSMs being concatenated is empty!");
			sub_nfsm_1 = connect_NFSM(sub_nfsm_1.m_init, sub_nfsm_1.m_final, sub_nfsm_2.m_init, sub_nfsm_2.m_final);
		}
		m_4_structure.insert(SymbolAndNFSMpairS(m_regexpr, sub_nfsm_1));
	}
	else { // regular expression is just one character
		m_4_structure.insert(SymbolAndNFSMpairS(m_regexpr, copy_nfsm(m_1_structure.at(m_regexpr.front()).m_init,
			m_1_structure.at(m_regexpr.front()).m_final)));
	}
	if (m_logging) m_logger << m_output_ws;
}
StateCouple Thompsons::transform() {
	try {
		first_stage();
		second_stage();
		third_stage();
		fourth_stage();
		fifth_stage();
		concatenate();
		return m_4_structure.at(m_regexpr);
	}
	catch (std::out_of_range e) {
		if (m_logging) m_logger << "Out of range: " << e.what();
	}
	catch (Nullptr e) {
		if (m_logging) m_logger << "Null pointer: " << e.what();
	}
	catch (Badargument e) {
		if (m_logging) m_logger << "Bad argument: " << e.what();
	}
	catch (...) {
		if (m_logging) m_logger << "Unknown error. Terminating...";
	}
}

StateCouple Thompsons::make_or_NFSM(State * s_init_1, State * s_final_1, State * s_init_2, State * s_final_2) {
	if (!s_init_1 || !s_final_1 || !s_init_2 || !s_final_2) throw Nullptr("make_or_NFSM: zero pointer passed as an argument.");
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
	if (!init || !final_s) throw Nullptr("copy_nfsm: null pointer passed as an argument");
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
StateCouple Thompsons::make_star_NFSM(State * s_init, State * s_final) {
	if (!s_init || !s_final) throw Nullptr("make_star_NFSM: null pointer as an argument.");
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
	if (!s_init || !s_final) throw Nullptr("make_plus_NFSM: null pointer as an argument.");
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
	if (!s_init || !s_final) throw Nullptr("make_simple_NFSM: null pointer as an argument.");
	//copy nfsm
	StateCouple copy = copy_nfsm(s_init, s_final);
	return copy;
}
StateCouple Thompsons::make_question_NFSM(State * s_init, State * s_final) {
	if (!s_init || !s_final) throw Nullptr("make_question_NFSM: null pointer as an argument.");
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
	if (!s_init_1 || !s_final_1 || !s_init_2 || !s_final_2) throw Nullptr("connect_NFSM: null pointer as an argument.");
	s_final_1->set_final(false);
	s_final_1->set_transition(m_t_id++, LAMBDA_CH, s_init_2);
	s_init_2->set_initial(false);
	s_final_2->set_final(true);
	return StateCouple(s_init_1, s_final_2);
}

StateCouple Thompsons::make_bracket_NFSM(std::string::iterator i, std::wstring & output_ws, char type) {
	if (*i != ')') throw Badargument("make_bracket_NFSM: bad argument passed.");
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
				std::string tmp_2 = read_subexpr_backwards(++j, OR_DELIM); j--;
				sub_nfsm_1 = copy_nfsm(m_or_structure.at(tmp_2).m_init, m_or_structure.at(tmp_2).m_final);
				sub_expr = tmp_2 + sub_expr;
			}
			else if (*j == BRACKET_DELIM && is_numeric(*(--j))) {
				std::string tmp_2 = read_subexpr_backwards(++j, BRACKET_DELIM); j--;
				sub_nfsm_1 = copy_nfsm(m_3_structure.at(tmp_2).m_init, m_3_structure.at(tmp_2).m_final);
				sub_expr = tmp_2 + sub_expr;
			}
			else if (!is_meta_char(*j)) {
				sub_expr.insert(sub_expr.begin(), *j);
				sub_nfsm_1 = copy_nfsm(m_1_structure.at(*j).m_init, m_1_structure.at(*j).m_final);
				j = prev_pos;
			}
			first_iter = false;
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
				sub_nfsm_2 = copy_nfsm(m_or_structure.at(tmp_2).m_init, m_or_structure.at(tmp_2).m_final);
				sub_expr = tmp_2 + sub_expr;
			}
			else if (*j == BRACKET_DELIM && is_numeric(*(--j))) {
				std::string tmp_2 = read_subexpr_backwards(++j, BRACKET_DELIM);
				sub_nfsm_2 = copy_nfsm(m_3_structure.at(tmp_2).m_init, m_3_structure.at(tmp_2).m_final);
				sub_expr = tmp_2 + sub_expr;
			}
			else if (!is_meta_char(*j)) {
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