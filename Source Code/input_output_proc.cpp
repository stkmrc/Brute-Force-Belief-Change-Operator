#include "input_output_proc.h"

//-------------------input------------------------//

vector<string> fileproc::read_FOBC_input(void)
//read FOBC formualas from input file
{
	ifstream if_FOBC("INPUT_FOBC.txt");
	vector<string> FOBC_in;
	
	if (if_FOBC.is_open()) 
	{
		int n = 0;
		string tempString;
		while (!if_FOBC.eof())
		{
			getline(if_FOBC, tempString);
			tempString.erase(std::remove_if(tempString.begin(), tempString.end(), ::isspace), tempString.end());
			if (!tempString.empty() && tempString[0] != '*') //skip comments and empty lines
			{
				// check if nr. of opening and closing brackets match
				if (!(count(tempString.begin(), tempString.end(), '(') == count(tempString.begin(), tempString.end(), ')')))
				{ 
					cout << "Check brackets. FOBC sentence in line " << (n + 1) << " was ignored.\n\n";
				}
				else
				{
					FOBC_in.push_back(tempString);
				}
			}
			n++;
		}

		if_FOBC.close();
	}
	else { cout << "FOBC input file failed to load\n\n"; }

	return(FOBC_in);
}

vector<fileproc::rawState> fileproc::get_predefStates(vector<string>& wnames)
//read predefined states from input file
{
	ifstream if_states("INPUT_States.csv");
	vector<fileproc::rawState> states_in;
	
	if (if_states.is_open()) 
	{
		auto n = 0;
		string tempString;
		bool headline = true;
		while (!if_states.eof())
		{
			getline(if_states, tempString);
			tempString.erase(std::remove_if(tempString.begin(), tempString.end(), ::isspace), tempString.end());
			if (!tempString.empty() && tempString[0] != '*' && tempString[0] != ';') //skip comments and empty lines
			{
				//remove spaces
				tempString.erase(std::remove_if(tempString.begin(), tempString.end(), ::isspace), tempString.end());

				string delimiter = ";";
				size_t pos = 0;
				vector<string> tokens;
				while ((pos = tempString.find(delimiter)) != string::npos) {
					tokens.push_back(tempString.substr(0, pos));
					tempString.erase(0, pos + delimiter.length());
				}
				tokens.push_back(tempString);
				vector<string> TPOtokens = tokens;
				TPOtokens.erase(TPOtokens.begin(), TPOtokens.begin() + 1); //ignore first column

				if (headline) //get #nr and names of worlds in first line with content
				{
					wnames = TPOtokens;
					headline = false;
				} 
				else
				{
					//check if line vector is fully filled with numbers
					if (TPOtokens.size() != wnames.size() || !fileproc::is_number(TPOtokens)) {
						cout << "****WARNING!****\nFaulty TPO predefined. Your predefined states (and therefore also edges) were ignored.\n";
						cout << "TPOs have to be fully defined! Check line " << (n + 1) << ".\n\n";
						return vector<rawState>();
					}
					else
					{	
						if (!doubleName(tokens[0], states_in)) //check if state name is used twice
						{
							fileproc::rawState tempstate;
							tempstate.name = tokens[0];

							vector<int> tempTPO;
							for (auto i = 0; i < TPOtokens.size(); i++)
								tempTPO.push_back(stoi(TPOtokens[i]));
							tempstate.TPO = fileproc::normalizeTPO(tempTPO);

							states_in.push_back(tempstate);
						}
						else {
							cout << "****WARNING!****\nState defined twice! Second definition of \"" << tokens[0];
							cout << "\" in line " << (n + 1) << " has been ignored.\n\n";
						}
					}
				}
			}	
			n++;
		}
		if_states.close();
	}
	else { cout << "Input file for states definition failed to load\n\n"; }
	return(states_in);
}
			
vector<fileproc::rawEdge> fileproc::get_predefEdges(const vector<string>& wnames, const vector<fileproc::rawState>& states)
//read predefined states from input file
{ 
	ifstream if_edges("INPUT_Edges.csv");
	vector<fileproc::rawEdge> edges_in;

	if (if_edges.is_open())
	{
		auto n = 0;
		string tempString;
		bool headline = true;
		while (!if_edges.eof())
		{
			getline(if_edges, tempString);
			tempString.erase(std::remove_if(tempString.begin(), tempString.end(), ::isspace), tempString.end());
			if (!tempString.empty() && tempString[0] != '*' && !emptyRow(tempString)) //skip comments and empty lines
			{
				tempString.erase(std::remove_if(tempString.begin(), tempString.end(), ::isspace), tempString.end());

				string delimiter = ";";
				size_t pos = 0;
				vector<string> tokens;
				while ((pos = tempString.find(delimiter)) != string::npos) {
					tokens.push_back(tempString.substr(0, pos));
					tempString.erase(0, pos + delimiter.length());
				}
				tokens.push_back(tempString);
				if (tokens.size() < 3) //check for minimum length of 
				{
					cout << "****WARNING!****\nFaulty line found in edge input file (empty data field)! Check line " << (n + 1) << endl;;
					cout << "Line was ignored.\n\n";
					n++;
					continue;
				}

				vector<string> data_tokens;
				data_tokens = tokens;
				data_tokens.erase(data_tokens.begin(), data_tokens.begin() + 2); // erase first 2 columns

				if (headline) {			
					if (wnames != data_tokens) //check if worlds match worlds in states input file
					{
						cout << "****WARNING!****\nNames or order of worlds does not match between the two .csv files!\n";
						cout << "Predefined Edges have been ignored.\n\n";
						return vector<fileproc::rawEdge>();
					}
					headline = false;					
				}
				else
				{
					//check if line is fully filled with boolean values and origin and destination are defined in states input file
					if (data_tokens.size() != wnames.size() || !is_bool(data_tokens) 
						|| !state_exists(tokens[0], states) || !state_exists(tokens[1], states))
					{
						cout << "****WARNING!****\nFaulty Edge predefined in line " << (n + 1) << " was ignored.\n";
						cout << "Edges have to be fully defined and states have to match the definition in the other .csv!\n\n";
						n++;
						continue;
					}
					else {
						fileproc::rawEdge tempEdge;
						tempEdge.origin = findIndex(tokens[0], states);
						tempEdge.edge.dest = findIndex(tokens[1], states);
						for (auto i = 0; i < data_tokens.size(); i++) { tempEdge.edge.alpha.push_back(data_tokens[i] == "true"); }
						edges_in.push_back(tempEdge);
					}
				}
			}
			n++;
		}
		if_edges.close();
	}
	else { cout << "Input file for edges definition failed to load\n\n"; }
	return(edges_in);
}

bool fileproc::is_number(const vector<string>& s) 
//check if all elements of string vector consist of only numbers
{
	for (auto i = 0; i < s.size(); i++)
	{
		if (s[i].empty() || std::find_if(s[i].begin(),
			s[i].end(), [](unsigned char c) { return !std::isdigit(c); }) != s[i].end())
		{
			return false;
		}
	}

	return true;
}

bool fileproc::is_bool(const vector<string>& s) 
//check if all elements of string vector consist of only "true" or "false"
{
	bool mintrue = false;
	for (auto x = 0; x < s.size(); x++)
	{
		if (s[x] != "true" && s[x] != "false")
		{
			return false;
		}
		if (s[x] == "true")
			mintrue = true;
	}

	return mintrue;
		
}

bool fileproc::state_exists(const string& token, const vector<fileproc::rawState>& states)
// check if input string is part of the name attributes in rawState vector
{
	for (fileproc::rawState st : states)
		if (token == st.name)
			return true;
	
	return false;
}

bool fileproc::emptyRow(const string& row) 
//check if string only consists of semicolons
{
	for (char c : row)
	{
		if (c != ';')
			return false;
	}
	return true;
}

bool fileproc::doubleName(const string& name, const vector<fileproc::rawState>& temp_states)
//check if input string is already part of a name attribute in the rawState vector
{
	for (fileproc::rawState s : temp_states)
		if (s.name == name)
			return true;
	return false;
}

int fileproc::findIndex(const string& s, const vector<fileproc::rawState>& temp_states)
//find the index of the element in the rawState vector, with a name attribute that matches the input string
{
	for (auto i = 0; i < temp_states.size(); i++)
	{
		if (temp_states[i].name == s)
			return i;
	}
}

vector<int> fileproc::normalizeTPO(const vector<int>& input)
//normalize input TPO
{
	int max_order = input.size() - 1;
	vector<int> output = input;
	for (auto i = 0; i < max_order; i++) {
		if (count(output.begin(), output.end(), i) == 0) {
			for (auto n = 0; n < output.size(); n++) {
				if (output[n] > i)
					output[n] = output[n] - 1;
			}
		}
	}
	return (output);
}

//-------------------output------------------------//

int fileproc::output_completedOperator(const changeOperator& CO, const vector<fileproc::rawEdge>& pdEs, const vector<fileproc::rawState>& pdSs, const vector<string>& wnames)
//output a changeOperator object as a .dot graph visualization file with different coloring for predefined states and edges specified in the three input vectors
{
	FILE* fout;
	fopen_s(&fout, "completedOperator.dot", "w");

	//----------------------------predefined states/edges-------------------------------------//
	fprintf(fout, "digraph CompletedOperator{\n\nrankdir=LR; concentrate=false;\n\n");
	fprintf(fout, "subgraph Predef {\nnode[color=blue];\nedge[color=blue];\n");
	for (fileproc::rawEdge pdE : pdEs)
	{
		fprintf(fout, "\"%s\" -> \"%s\" [label=<<font color=\"blue\">%s</font>>]\n", getState(pdE.origin, pdSs).c_str(), getState(pdE.edge.dest, pdSs).c_str(), genLabel(pdE.edge.alpha, wnames).c_str());
	}
	
	fprintf(fout, "node[color=blue shape=box style=dashed];\nedge[color=blue style=dashed arrowhead=none];\n");
	for (fileproc::rawState pdS : pdSs)
	{
		fprintf(fout, "\"%s\" -> \"%s\"\n", genTPOLabel(pdS.TPO, wnames).c_str(), pdS.name.c_str());
	}
	fprintf(fout, "}\n\n");

	//----------------------------inserted states/edges-------------------------------------//
	fprintf(fout, "node[style=filled];\nedge[color=grey];\n");
	for (auto i = 0; i < CO.states.size(); i++) {
		for (changeOperator::state::edge x : CO.states[i].edges)
		{
			if (!israwEdge(x, i, pdEs))
				fprintf(fout, "\"%s\" -> \"%s\" [label=\"%s\"]\n", getState(i, pdSs).c_str(), getState(x.dest, pdSs).c_str(), genLabel(x.alpha, wnames).c_str());
		}
	}
	fprintf(fout, "node[color=black shape=box style=dashed];\nedge[color=black style=dashed arrowhead=none];\n");
	for (auto i = pdSs.size(); i < CO.states.size(); i++)
		fprintf(fout, "\"%s\" -> \"%s\"\n", genTPOLabel(CO.states[i].TPO, wnames).c_str(), getState(i, pdSs).c_str());
			
	fprintf(fout, "}\n");
	fclose(fout);

	return EXIT_SUCCESS;
}

bool fileproc::israwEdge(const changeOperator::state::edge& e,const int& i, const vector<fileproc::rawEdge>& pdEs)
{
	for (fileproc::rawEdge rE : pdEs)
	{
		if (rE.origin == i && rE.edge.alpha == e.alpha)
			return true;
	}
	return false;
}

string fileproc::genLabel(const vector<bool>& alpha, const vector<string>& wnames)
//convert internal alpha representation (vector of bools) to plain text for improved human readability
{
	string output ="{";
	for (auto i = 0; i < alpha.size(); i++)
	{
		if (alpha[i])
		{
			if (output != "{")
				output += ", ";
			output += wnames[i];
		}
	}
	output += "}";
	return output;
}

string fileproc::genTPOLabel(const vector<int>& TPO, const vector<string>& wnames)
//convert internal TPO representation (vector of ints) to plain text for improved human readability
{
	string output = "";
	int maxElement = *std::max_element(TPO.begin(), TPO.end());
	for (auto i = 0; i <= maxElement; i++)
	{
		vector<bool> boolTPO;
		for (auto x = 0; x < TPO.size(); x++) { boolTPO.push_back(TPO[x] == i); }
		output += fileproc::genLabel(boolTPO, wnames);
		if (i != maxElement)
			output += " < ";
	}
	return output;
}

string fileproc::getState(const int& index, const vector<fileproc::rawState>& predefStates)
//get predefined state name if existent, otherwise generate a state name
{
	string sname;
	if (index < predefStates.size())
	{
		sname = predefStates[index].name;
	}
	else
	{
		sname = "s";
		sname += to_string(index - predefStates.size());
		sname += "*";
	}
	return sname;
}
