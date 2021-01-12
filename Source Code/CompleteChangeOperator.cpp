#include "input_output_proc.h"
#include <chrono>

int main() 
{
	auto start = chrono::high_resolution_clock::now();
	srand(time(NULL));

	vector<string> FOBC_input = fileproc::read_FOBC_input(); // read FOBC input from .txt file
	vector<SyntaxTree> FOBC_trees(FOBC_input.size());;

	for (auto i = 0; i < FOBC_trees.size(); i++) {
		switch (FOBC_trees[i].parse_FOBC(FOBC_input[i])) { // print syntax tree or error (debugging)
			case 0: 
				cout << "SyntaxTree " << i+1 << "/" << FOBC_trees.size() << ":\n";
				SyntaxTree::createExpressionString(FOBC_trees[i].root, 0);
				cout << endl << endl;
				break;
			case 1:
				cout << "The following FOBC input has been ignored due to invalid syntax:\n" << FOBC_input[i] << endl << endl;
		}
	}

	vector<string> wnames;
	vector<fileproc::rawState> predefStates = fileproc::get_predefStates(wnames); //get predefined states from first .csv file
	vector<fileproc::rawEdge> predefEdges = fileproc::get_predefEdges(wnames, predefStates); //get predefined edges from second .csv file
	if (wnames.size() < 2) { //check if number of worlds is at least 2
		cout << "A Minimum of 2 worlds is required to model a believe change.\n";
		cin.get();//leave console open
		return 1;
	}

	changeOperator CO(wnames.size()); //create changeOperator
	for (fileproc::rawState rS : predefStates) { CO.addState(rS.TPO); } //add predefined states
	for (fileproc::rawEdge rE : predefEdges) { CO.addEdge(rE.origin, rE.edge.dest, rE.edge.alpha); } //add predefined states


	switch (CO.complete(FOBC_trees)) { //complete operator and check result

		case 1: //failure
			cout << "\n\n------------------------COMPLETION FAILED------------------------\n\n";
			cout << "There exists no complete operator for your input.\n";
			cin.get();//leave console open
			return 1;

		case 0:	//success

			//CO.debugOutput(); //visualize completed operator in system console

			//output summary in system console
			cout << "\n\n----------------------SUCESSFULLY FINISHED----------------------\n\n";
			cout << "Total States: " << CO.states.size() << endl;
			cout << "Anzahl gefundene einzelne Graphen: " << CO.graphs << " * 10^" << CO.graphs_pow <<endl;
			auto stop = chrono::high_resolution_clock::now();
			auto duration = chrono::duration_cast<chrono::seconds>(stop - start);
			cout << "Execution time: " << duration.count() << "s\n\n";

			//output operator as .dot file for graph visualization
			fileproc::output_completedOperator(CO, predefEdges, predefStates, wnames);

			//cin.get(); //leave console open
			return 0;
	}

}