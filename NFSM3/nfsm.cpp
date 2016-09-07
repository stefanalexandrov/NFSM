#include "stdafx.h"
#include "nfsm.h"
#include "NFSM3.h"
#include "NFSM3Dlg.h"
#include "DlgProxy.h"
#include "afxdialogex.h"
#include "ThompsonsAlgImpl.h"
#include "State.h"
#include "Transition.h"
#include "Helpers.h"
#include "SimpleLogger.h"
#include "..\Thompsons\Thompsons_i.c"
#include "..\Thompsons\Thompsons_i.h"


RUN::RUN(NFSM* machine, CWnd* output_wnd) {
	m_nfsm = machine;
	m_nfsms.push_back(*machine);
	m_nfsms.reserve(MAX_NUMBER_OF_NFSM_COPIES);
	m_output = output_wnd;
}
NFSM::NFSM() : m_states{ nullptr }, m_logger{ SimpleLogger::GetInstance() }, m_logging{false},
    m_constructed{ false }, m_valid{true} {}
NFSM::NFSM(const NFSM& nfsm) :
    m_states{ nfsm.m_states },
	m_current{ nfsm.m_current },
	m_constructed{ nfsm.m_constructed },
	m_valid{ nfsm.m_valid },
	m_output{ nfsm.m_output },
	m_s_id{ nfsm.m_s_id },
	m_logger{ SimpleLogger::GetInstance() },
	m_logging{ nfsm.m_logging }
{}
NFSM& NFSM::operator=(const NFSM& nfsm) {
	m_states = m_states;
	m_current = nfsm.m_current;
	m_constructed = nfsm.m_constructed;
	m_valid = nfsm.m_valid;
	m_output = nfsm.m_output;
	m_s_id = nfsm.m_s_id; 
	m_logging = nfsm.m_logging;
	return *this;
}
NFSM::NFSM(NFSM&& nfsm):
    m_states{ nfsm.m_states }, //just grab elements without copying
	m_current{ nfsm.m_current }, m_constructed{ nfsm.m_constructed },
    m_valid{ nfsm.m_valid }, m_output{ nfsm.m_output }, m_s_id{ nfsm.m_s_id },
	m_logger{ SimpleLogger::GetInstance() }, m_logging{nfsm.m_logging}
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
	m_logging = nfsm.m_logging;
	return *this;
}


int NFSM::construct() {
	//adding Dll code for transformation
	HRESULT hr;
	IThompsonsAlg* ITalg = nullptr;

	std::wstring output_ws = read_output_wnd(m_output);
	output_ws.append(_T("Starting Transformation... \r\n"));
	output_ws.append(_T("Initializing transformation algorithm... "));
	//intilize COM
	hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

	if (SUCCEEDED(hr))
	{
		hr = CoCreateInstance(CLSID_ThompsonsAlg, NULL,
			CLSCTX_INPROC_SERVER,
			IID_IThompsonsAlg, (void**)&ITalg);

		if (SUCCEEDED(hr))
		{
			output_ws.append(_T("Success! \r\n"));
			UINT InitState;
			UINT FinalState;
			UINT NStates;
			UINT States;
			UINT StatesArr;
			UINT SatesArrSize;
			_bstr_t log(L"");
			ITalg->SetRegExpr(_bstr_t(m_regexpr.c_str()).GetBSTR());
			ITalg->Transform(&InitState, &FinalState);
			//Get Copy of the States array
			ITalg->GetStatesArray(&StatesArr, &SatesArrSize);
			ITalg->GetNumberOfStates(&NStates);
			m_s_id = NStates - 1;
			/*
			std::unique_ptr<State, std::default_delete<State[]>> states { new State[SatesArrSize] };
			for (int i = 0; i < SatesArrSize; i++)
				states.get()[i] = ((State*)StatesArr)[i];
			
			m_states = std::move(states);
			m_nfsm = StateCouple(find_initial_state(m_states.get(), SatesArrSize), find_final_state(m_states.get(), SatesArrSize));
			*/
			m_states = copy_nfsm((State*)InitState, (State*)FinalState, SatesArrSize, NStates);
			m_nfsm = StateCouple(find_initial_state(m_states.get(), NStates), find_final_state(m_states.get(), NStates));
			m_current = m_nfsm.m_init;
			
			ITalg->GetLog2((UINT*)log.GetAddress());
			std::wstring wstmp(log.GetBSTR(), SysStringLen(log.GetBSTR()));
			output_ws.append(wstmp);

			ITalg->Release();
		} else
			output_ws.append(_T("Failure! \r\n"));

	}
	// Uninitialize COM
	CoUninitialize();
	//

	if (m_logging) m_logger << output_ws;
	output_ws.append(_T("Done... \r\n "));
	if (m_logging) m_logger << output_ws;
	m_output->SetWindowTextW(output_ws.c_str());
	m_constructed = true;
	return 0;
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
		
		for (auto tt = runs.begin(); tt != runs.end(); tt++) {

			run_lambda(NULL, &(*tt), false, &visited_states, &n_invalidated); // first stage: perform all possible §-transitions
			// lambda-transitions are creators of the copies
			//actually delete nfsms 
			while (n_invalidated-- > 0)
				for (auto j = tt->begin(); j != tt->end(); j++) {
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
		for (auto tt = runs.begin(); tt != runs.end(); tt++) {
			tmp.insert(tmp.end(), tt->begin(), tt->end());
		}
				// create a set of symbols of all transitions of current states reached
				set_of_symbols.clear(); // each time a new set of symbols
				for (auto it = tmp.begin(); it != tmp.end(); it++) {
					for (auto tr = it->get_current()->get_out().begin(); tr != it->get_current()->get_out().end(); tr++) {
						if (tr->get_symbol() != LAMBDA_CH)
							set_of_symbols.insert(tr->get_symbol());
					}
				}
			
				for (auto it = set_of_symbols.begin(); it != set_of_symbols.end(); it++) {
					//create NFSM vector for each symbol, merge NFSM from other NFSM vectors
					std::vector<NFSM> tmp_2 = tmp;
					// normal transitions are destructors of copies
					for (auto j = tmp_2.begin(); j != tmp_2.end(); j++) {
						transition_for_symbol(&tmp_2, &(*j), *it, false, &n_invalidated, true);
					}
					//actually delete nfsms 
					while (n_invalidated-- > 0)
						for (auto j = tmp_2.begin(); j != tmp_2.end(); j++) {
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

TransType run_lambda(RUN * obj, std::vector<NFSM> * p_nfsms,
	bool last_ch, std::set<int> * visited_s = NULL, int * n_invalidated = NULL) {
	TransType t_type;
	int n_invalidated_2 = 0;
	int max_t = 20;
	std::set<int> visited;
	std::set<int>::iterator cur, no_rep;
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

	for (auto j = nfsm->begin(); j != nfsm->end(); j++) {
		if (j->get_current()->is_final() && !last_ch) {
			j->set_invalid();
			n_invalidated_2++;
		}
	}
	//actually delete nfsms 
	while (n_invalidated_2-- > 0)
		for (auto j = nfsm->begin(); j != nfsm->end(); j++) {
			if (!j->is_valid() && j->get_current()->is_final()) {
				nfsm->erase(j);
				break;
			}
		}
	return TransType::NOT_FINAL_LAMBDA;
}
void NFSM::optimize(Optimizer& opt) {
	opt.optimize(m_nfsm.m_init, m_states, m_s_id);
}
void SuperflousStatesRemover::optimize(State* init, std::shared_ptr<State> states, int m_s_id) {
	if (m_logging) m_logger << "Start optimization... \n";
	int n_removed = 0;
	int start_id = init->get_id();

	int num = 5;
	while (num--) {
		for (int i = start_id; i <= m_s_id; i++) {
			State * st = &states.get()[i];
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
				states.get()[i].set_empty(true);
				n_removed += 1;
			}
		}
	}
	//start with many lambda-inputs, one output
	num = 5;
	while (num--) {
		for (int i = start_id; i <= m_s_id; i++) {
			State * st = &states.get()[i];
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
				states.get()[i].set_empty(true);
				n_removed += 1;
			}
		}
	}
	if (m_logging) m_logger << "States removed: " << n_removed << ". Done.\n";
}

int NFSM::save(NFSMSaver& saver) {
	return saver.save("nfsm.txt", m_nfsm.m_init, m_s_id);
}
int DOTSaver::save(std::string file_name, State* start, int m_s_id) {
	// open a file in write mode.
	std::ofstream out_file;
	std::string graph, graph_tmp;
	out_file.open(file_name);
	// header
	out_file << "strict digraph {" << std::endl;
	out_file << "	rankdir = LR" << std::endl;

	std::vector<State *> current_s;
	current_s.push_back(start);
	std::vector<State *> current_s_new;
	int size = m_s_id/2;
	while (--size > 0) {
		for (auto j = current_s.begin(); j != current_s.end(); j++) {
			for (auto i = (*j)->get_out().begin(); i != (*j)->get_out().end(); i++) {
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
	return 0;
}

TransType RUN::transition_for_symbol(std::vector<NFSM> * p_nfsms, NFSM * nfsm,
	char input, bool last_ch, int * n_inval, bool formal) {
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

