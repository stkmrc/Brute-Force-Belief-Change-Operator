#pragma once

#include "FOBC_parser.h"
#include "model_checking.h"
#include <cmath>

#define reducedDiskSpace false 
//when "false", TPOs will be created quicker but stored in a (maybe large) table
//when "true", TPOs will take longer to create, but not all orders have to be stored

#define outputMode "single"
//when "single", one possible completed operator will be created (always starts checking states in numerical order)
//when "random", a random possible completed operator will be created
//when "all", a completed operator containing all possible edges will be created

class changeOperator {

public:

	changeOperator(const int& world_count) {
		this->worlds = world_count;
		graphs = 1;
		graphs_pow = 0;
	}

	void addState(const vector<int>& TPO);

	void addEdge(const int& orig, const int& dest, const vector<bool>& alpha);

	int complete(const vector<SyntaxTree>& FOBC_Trees);

	struct state {

		struct edge {
			vector<bool> alpha;
			int dest;

			edge() : alpha(), dest() {}
			edge(const int& dest_in, const vector<bool>& alpha_in) : alpha(alpha_in), dest(dest_in) {}
		};

		vector<int> TPO;
		vector<edge> edges;

		state() : TPO(), edges() {}
		state(const vector<int>& TPO_in) : TPO(TPO_in), edges() {}

	};

	vector <state> states;
	int graphs;
	int graphs_pow;

	void debugOutput(void) { 
	//output change operator to system console (debugging)
		for (auto x = 0; x < this->states.size(); x++) 
		{
			cout << "TPO State " << x << ": ";
			vector<int> path = this->states[x].TPO;
			for (auto i = 0; i < path.size(); i++)
				cout << path[i] << "  ";
			cout << endl;
			cout << "Edges: " << this->states[x].edges.size() << endl;

			for (auto y = 0; y < this->states[x].edges.size(); y++) {
				cout << "\tEdge " << y << ":" << endl;;
				cout << "\t\tDestination: " << this->states[x].edges[y].dest << endl;;
				cout << "\t\tAlpha: ";
				vector<bool> path2 = this->states[x].edges[y].alpha;
				for (auto j = 0; j < path2.size(); j++)
					cout << path2[j] << "  ";
				cout << endl;
			}
		}
	}
	
private:

	vector<vector<int>> TPOtable;
	int worlds;

	void genTPOtable(const vector<int>& root_TPO);

	bool genTPOs_v2(int& index, vector<int>& TPO);

	vector<int> getTPO(const int& index);
	
	bool checkTPO(const vector<int>& TPO);

	vector<bool> generateSubsets(const int& index);

	void completeStates(void);

	int completeEdges(const vector<SyntaxTree>& FOBC_cond);

	int bruteForce(const int& origState, const vector<bool>& alpha, const vector<SyntaxTree>& FOBC_cond);

	int poweroften(int x);

};