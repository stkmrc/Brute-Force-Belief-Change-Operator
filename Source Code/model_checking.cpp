#include "model_checking.h"


bool model_check::checkSubTree(const SyntaxTree::node* root, const vector<bool>& alpha, const vector<int>& TPO1, const vector<int>& TPO2, unordered_map<string, int>& var_assg)
//evaluate if a given formula as a syntax tree is fulfilled by given alpha, TPO of origin state, TPO of destination state and variable assignment
{
	if (root == nullptr)
		return true;

	if (root->type == "Mod") {
		if (alpha[var_assg[root->args[0]]])
			return true;
	}
	else if (root->type == "TPO1") {
		if (TPO1[var_assg[root->args[0]]] <= TPO1[var_assg[root->args[1]]])
			return true;
	}
	else if (root->type == "TPO2") {
		if (TPO2[var_assg[root->args[0]]] <= TPO2[var_assg[root->args[1]]])
			return true;
	}
	else if (root->type == "~") {
		return !checkSubTree(root->left, alpha, TPO1, TPO2, var_assg);
	}
	else if (root->type == "E") {
		for (auto x = 0; x < TPO1.size(); x++) {
			var_assg[root->args[0]] = x;
			if (checkSubTree(root->left, alpha, TPO1, TPO2, var_assg))
				return true;
		}
	}
	else if (root->type == "A") {
		for (auto x = 0; x < TPO1.size(); x++) {
			var_assg[root->args[0]] = x;
			if (!checkSubTree(root->left, alpha, TPO1, TPO2, var_assg))
				return false;
		}

		return true;
	}
	else{
		bool left = checkSubTree(root->left, alpha, TPO1, TPO2, var_assg);
		bool right = checkSubTree(root->right, alpha, TPO1, TPO2, var_assg); 

		if (root->type == "&") 
			return (left && right);

		else if (root->type == "|")
			return (left || right);
	
		else if (root->type == "=>")
			return (!left || right);
	
		else if (root->type == "<=>")
			return (left == right);
	}

	return false;
}

bool model_check::c_OccurCheck(const SyntaxTree::node* root)
//recursive check if the constant c is part of a given syntax tree
{
	if (root == nullptr)
		return false;

	if ( find(root->args.begin(), root->args.end(), "c") != root->args.end())
		return true;
	else
		return(c_OccurCheck(root->left) || c_OccurCheck(root->right));
}

vector<string> model_check::getFreeVariables(const SyntaxTree::node* root)
//recursively collect all free variables in a given syntax tree as a vector of strings of variable names
{
	if (root == nullptr)
		return vector<string>();

	vector<string> output;

	if (root->type == "Mod") {
		if (root->args[0] != "c")
			output.push_back(root->args[0]);
	}
	else if (root->type == "TPO1" || root->type == "TPO2") {
		if (root->args[0] != "c")
			output.push_back(root->args[0]);
		if (root->args[1] != "c")
			output.push_back(root->args[1]);
	}
	else {
		vector<string> temp_Variables_L = getFreeVariables(root->left);		
		if (SyntaxTree::isQuantor(root->type) || (root->type == "~")) {
			for (auto i = 0; i < temp_Variables_L.size(); i++) {
				if (temp_Variables_L[i] != root->args[0])
					output.push_back(temp_Variables_L[i]);
			}
		}
		else {
			output = getFreeVariables(root->right);
			for (auto i = 0; i < temp_Variables_L.size(); i++) {
				if (count(output.begin(), output.end(), temp_Variables_L[i]) == 0)
					output.push_back(temp_Variables_L[i]);
			}
		}
	}
	
	return output;

}

bool model_check::checkAllVarAssigns(const vector<SyntaxTree>& FOBC_Trees, const vector<bool>& alpha, const vector<int>& TPO1, const vector<int>& TPO2, const int& c_world)
//check all variable assignments for given combination of syntax trees, alpha, c assignment, TPO of destination and TPO of origin
{
	for (auto k = 0; k < FOBC_Trees.size(); k++) {
	//loop 5: check all FOBC sentences
		vector<string> freeVariables = model_check::getFreeVariables(FOBC_Trees[k].root);
		unordered_map<string, int> var_assignment;

		for (auto index = 0; index < pow(TPO1.size(), freeVariables.size()); index++) {
		//loop 6: check all possible variable assignments
			var_assignment = model_check::genVarAssigns(TPO1.size(), freeVariables, index);
			var_assignment["c"] = c_world;
			if (!model_check::checkSubTree(FOBC_Trees[k].root, alpha, TPO1, TPO2, var_assignment)) {
				return false;
			}
		}
	}

	return true;
}

unordered_map<string, int> model_check::genVarAssigns(const int& worlds, const vector<string>& freeVars, const int& index)
//generate a possible variable assignment with given index for given vector of free variables as strings
{
	int num = index;
	unordered_map<string, int> var_assign;
	for (auto i = 0; i < freeVars.size(); i++) {
		var_assign[freeVars[i]] = num % worlds;
		num = num / worlds;
	}
	return var_assign;
}
