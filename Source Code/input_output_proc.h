#pragma once

#include <iostream>
#include <fstream>
#include "belief_change_space.h"

#ifdef __unix
#define fopen_s(pFile,filename,mode) ((*(pFile))=fopen((filename),(mode))) == NULL
#endif

namespace fileproc {

	//-------------------input------------------------//

	struct rawEdge {
		changeOperator::state::edge edge;
		int origin = 0;
	};

	struct rawState {
		vector<int> TPO;
		string name;
	};

	vector<string> read_FOBC_input(void);

	vector<rawState> get_predefStates(vector<string>& wnames);

	vector<rawEdge> get_predefEdges(const vector<string>& wnames, const vector<fileproc::rawState>& states);

	bool is_number(const vector<string>& s);

	bool is_bool(const vector<string>& s);

	bool state_exists(const string& token, const vector<fileproc::rawState>& states);

	bool emptyRow(const string& row);

	bool doubleName(const string& name, const vector<fileproc::rawState>& temp_states);

	int findIndex(const string& s, const vector<fileproc::rawState>& temp_states);

	vector<int> normalizeTPO(const vector<int>& input);

	//-------------------output------------------------//

	int output_completedOperator(const changeOperator& CO, const vector<fileproc::rawEdge>& pdEs, const vector<fileproc::rawState>& pdSs, const vector<string>& wnames);

	bool israwEdge(const changeOperator::state::edge& e, const int& i, const vector<fileproc::rawEdge>& pdEs);
	
	string genLabel(const vector<bool>& alpha, const vector<string>& wnames);

	string genTPOLabel(const vector<int>& TPO, const vector<string>& wnames);

	string getState(const int& index, const vector<fileproc::rawState>& predefStates);

}




