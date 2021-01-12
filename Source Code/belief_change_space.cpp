#include "belief_change_space.h"

int changeOperator::complete(const vector<SyntaxTree>& FOBC_Trees)
//complete states and edges of this operator object
{
	completeStates(); //loop 1 for adding missing states
	cout << "\n\noutputMode = " << outputMode << endl;
	cout << "reducedDiskSpace = " << reducedDiskSpace << endl << endl;
	return (completeEdges(FOBC_Trees));	 //loop 2 for adding missing edges
}

/****************State/TPO Functions*************************/

void changeOperator::completeStates(void)
//complete states of this operator object
{
	int predef_states = this->states.size();
	vector<vector<int>> predef_TPOs;
	for (auto k = 0; k < predef_states; k++) {
		predef_TPOs.push_back(this->states[k].TPO);
	}
	
	if (reducedDiskSpace) { //generate TPOs memory (table) or cpu efficient
		for (auto i = 0; i < pow(this->worlds, this->worlds); i++)
		{
			vector<int> temp_order(this->worlds, 0);
			if (genTPOs_v2(i, temp_order) && count(predef_TPOs.begin(), predef_TPOs.end(), temp_order) == 0)
			//for all possible orders, that are not predefined, create a state
			{
				addState(temp_order);
			}
		}
	}
	else {
		vector<int> initialTPO(this->worlds, 0);
		this->TPOtable.push_back(initialTPO);
		genTPOtable(initialTPO);
		for (auto i = 0; i < this->TPOtable.size(); i++)
		{
			vector<int> temp_order = this->TPOtable[i];
			if (count(predef_TPOs.begin(), predef_TPOs.end(), temp_order) == 0)
			//for all possible orders, that are not predefined, create a state
			{
				addState(temp_order);
			}
		}
	}
}

void changeOperator::addState(const vector<int>& TPO) 
//create new state for this object with the input vector as TPO
{
	state new_state(TPO);
	this->states.push_back(new_state);
}

void changeOperator::genTPOtable(const vector<int>& root_TPO)
//generate table for this object containing all possible TPOs depending on the nr. of worlds
//tree-like search pattern (cpu efficient, not memory efficient)
{
	int max_order = this->worlds - 1;
	for (auto i = 0; i < this->worlds; i++) {
		if ((root_TPO[i] < max_order) && (count(root_TPO.begin(), root_TPO.end(), root_TPO[i]) > 1)) {
			vector<int> mod_TPO = root_TPO;
			mod_TPO[i]++;
			if (count(this->TPOtable.begin(), this->TPOtable.end(), mod_TPO) == 0) {
				this->TPOtable.push_back(mod_TPO);
				genTPOtable(mod_TPO);
			}
		}
	}
	return;
}

bool changeOperator::genTPOs_v2(int& index, vector<int>& TPO)
//generate possible TPO with input index (memory efficient, not cpu efficient)
{
	while (index < pow(this->worlds, this->worlds))
	{
		TPO = getTPO(index);
		if (checkTPO(TPO))
			return true;
		else
			index++;
	}
	return false;

}

vector<int>changeOperator::getTPO(const int& index)
//creates candidates for TPOs (vector of integers between 0 and nr. of worlds - 1)
{
	int num = index;
	vector<int> output(this->worlds, 0);
	for (auto i = 0; i < this->worlds; i++) 
	{
		output[i] = num % this->worlds;
		num = num / this->worlds;
		if (num == 0)
			break;
	}

	return output;
}

bool changeOperator::checkTPO(const vector<int>& TPO)
//check if input TPO candidate is a standardized TPO
{
	for (auto i = 0; i < this->worlds - 1; i++) {
		if (count(TPO.begin(), TPO.end(), i) == 0) {
			for (auto n = 0; n < TPO.size(); n++) {
				if (TPO[n] > i)
					return false;
			}
		}
	}

	return true;
}

/****************Edge/Alpha Functions***********************/

int changeOperator::completeEdges(const vector<SyntaxTree>& FOBC_cond) 
//complete edges of this operator object
{
	for (auto i = 0; i < this->states.size(); i++) { //check all origin states
		
		int predef_edges = this->states[i].edges.size();
		vector<vector<bool>> predef_alphas;
		for (auto k = 0; k < predef_edges; k++) {
			predef_alphas.push_back(this->states[i].edges[k].alpha);
		}

		int maxSubsets = pow(2, this->worlds);
		for (auto j = 1; j < maxSubsets; j++) {
			vector<bool> new_alpha = generateSubsets(j);
			if (count(predef_alphas.begin(), predef_alphas.end(), new_alpha) == 0) 
			{ 
				//check all alphas for each origin state that are not predefined
				if (bruteForce(i, new_alpha, FOBC_cond) == 1)
					return 1;
			}
		}
	}

	return 0;
}

int changeOperator::bruteForce(const int& origState, const vector<bool>& alpha, const vector<SyntaxTree>& FOBC_cond)
//try all variations of c and destination state for given alpha and origin to fulfill input syntax trees
{
	vector<int> TPO1 = this->states[origState].TPO;
	int c_assign = 1;
	int edgesfound = 0;
	vector<bool> dest_checked(states.size(), false);
	
	//if c is not part of any input FOBC sentence it does not have to be varied
	for (auto l = 0; l < FOBC_cond.size(); l++) { 
		if (model_check::c_OccurCheck(FOBC_cond[l].root)) {
			c_assign = this->worlds;
			break;
		}
	}

	for (int m = 0; m < this->states.size(); m++)  //loop 3: try all destination states
	{
		int dest;
		if (outputMode == "random")
			while (true)
			{
				int r = rand() % states.size(); //select random destination
				if (!dest_checked[r]) {
					dest = r;
					dest_checked[r] = true;
					break;
				}
			}
		else
			dest = m;

		/*if (dest == origState)
			continue;*/ //when origin must not be destination state

		vector<int> TPO2 = this->states[dest].TPO;
		for (auto j = 0; j < c_assign; j++) //loop 4: try all assignements for c if it is part of the FOBC sentences
		{ 
			if (model_check::checkAllVarAssigns(FOBC_cond, alpha, TPO1, TPO2, j))
			//check all variable assignments for given combination of syntax trees, alpha, c assignment, TPO of destination and TPO of origin
			{
				addEdge(origState, dest, alpha);
				if (outputMode != "all") 
					return 0;
				else 
					edgesfound++;
					break;
			}
		}
	}

	if (edgesfound > 0)	{
		this->graphs *= edgesfound;
		if (this->graphs >= 100) {
			this->graphs_pow += (poweroften(this->graphs) - 1);
			this->graphs /= pow(10, poweroften(this->graphs) - 1);
		}
		return 0;
	}
	else {
		this->graphs = 0;
		this->graphs_pow = 0;
		return 1;
	}
}

int changeOperator::poweroften(int x)
{
	return x >= 10 ? poweroften(x / 10) + 1 : 0;
}
	
void changeOperator::addEdge(const int& orig, const int& dest, const vector<bool>& alpha)
//add edge to object and fill attributes origin, destination and alpha with input
{
	state::edge new_edge(dest, alpha);
	this->states[orig].edges.push_back(new_edge);
	return;

}

vector<bool> changeOperator::generateSubsets(const int& index)
//creates possible subsets of nr. of worlds of object (vector of bools)
{
	int num = index;
	vector<bool> output(this->worlds, 0);
	for (auto i = 0; i < this->worlds; i++) {
		output[i]= num % 2;
		num = num / 2;
		if (num == 0)
			break;
	}
	
	return output;
}






