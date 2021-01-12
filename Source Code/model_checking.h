#pragma once

#include "belief_change_space.h"
#include <unordered_map>

namespace model_check {

	bool checkSubTree(const SyntaxTree::node* root, const vector<bool>& alpha, const vector<int>& TPO1, const vector<int>& TPO2, unordered_map<string, int>& var_assg);

	bool c_OccurCheck(const SyntaxTree::node* root);

	vector<string> getFreeVariables(const SyntaxTree::node* root);

	bool checkAllVarAssigns(const vector<SyntaxTree>& FOBC_Trees, const vector<bool>& alpha, const vector<int>& TPO1, const vector<int>& TPO2, const int& c_world);

	unordered_map<string, int> genVarAssigns(const int& worlds, const vector<string>& freeVars, const int& index);
}