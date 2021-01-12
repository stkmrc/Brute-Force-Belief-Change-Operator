#include "FOBC_parser.h"

int SyntaxTree::parse_FOBC (const string& s) 
//parse FOBC to SyntaxTree
{
		this->root = createSyntaxTree(tokenize(s));
		if (this->root == nullptr) 
			{ return 1; }
		return 0;
}

vector<string> SyntaxTree::tokenize(const string& s) 
//tokenize string to vector of string tokens
{
	vector<string> result;

	const char* exp = s.c_str();
	unsigned length = (unsigned)s.length();

	for (auto i = 0; i < length; i++) {
		if (exp[i] == '(') {
			result.push_back("(");
		}
		else if (exp[i] == ')') {
			result.push_back(")");
		}
		else if (exp[i] == '~') {
			result.push_back("~");
		}
		else if (exp[i] == '&') {
			result.push_back("&");
		}
		else if (exp[i] == '|') {
			result.push_back("|");
		}
		else if (exp[i] == '=' && exp[i + 1] == '>') {
			result.push_back("=>");
			i++;
		}
		else if (exp[i] == '<' && exp[i + 1] == '=' && exp[i + 2] == '>') {
			result.push_back("<=>");
			i = i + 2;
		}
		else {
			auto j = i + 1;
			while (exp[j] != ')') { j++; };
			string temp(exp, i, (j - i) + 1);
			result.push_back(temp);
			i = j;
		}

	}

	return result;
}

SyntaxTree::node* SyntaxTree::createSyntaxTree(const vector<string>& expression) 
//create tree of tokenized expression by operator precedences
{
	stack<string> op;
	stack<node*> result;

	for (auto i = 0; i < expression.size(); i++) {
		string temp = expression[i];
		if (isOperator(temp)) {
			if (op.empty()) {
				op.push(temp);
			}
			else if (temp == "~" || temp == "(" || isQuantor(temp)) {
				op.push(temp);
			}
			else if (temp == ")") {
				while (op.top() != "(") {
					if (addToStack(op.top(), result) == 1)
						return nullptr;
					op.pop();
				}
				op.pop();
			}
			else if (operatorPrecedence(temp) > operatorPrecedence(op.top())) { 
				//collect operators on stack as long as precedence of next operator is lower
				op.push(temp);
			}
			else if (operatorPrecedence(temp) <= operatorPrecedence(op.top())) {
				//when operator precedence is lower or equal create subtree from last operators with higher precedence
				if ((op.top() != "~" && !isQuantor(op.top()) && result.size() < 2)) {
					return nullptr; //avoid out of range
				}
				while (!op.empty() && operatorPrecedence(temp) <= operatorPrecedence(op.top())) {
					if (addToStack(op.top(), result) == 1)
						return nullptr;
					op.pop();
				}
				op.push(temp);
			}
		}
		else { //predicates are directly added to stack, since no subtrees have to be created
			if (addToStack(temp, result) == 1)
				return nullptr;
		}
	}

	while (!op.empty()) {
		if (addToStack(op.top(), result) == 1)
			return nullptr;
		op.pop();
	}

	return result.top();
}

int SyntaxTree::addToStack(const string& op, stack<node*>& operandStack) 
//create subtree for operator, delete node pointers of operands from stack and replace with pointer to subtree
{
	node* root = createNode(op); //create node for the operator or operand

	if (root == nullptr) { //delete all nodes already created
		for (auto i = 0; i < operandStack.size(); i++) {
			node* temp = operandStack.top();
			deleteBranch(temp);
			operandStack.pop();
		}
		return 1;
	}
	
	if (isOperator(op)) { //if node is an operator process the operator based on unary or binary operation
		if (op == "~" || isQuantor(op)) { 
			node* operand = operandStack.top(); //get node for the operand
			operand->parent = root;	
			operandStack.pop(); //pop the operand stack
			root->left = operand; //assign the operand to the left of the root operator
			root->right = nullptr;
		}
		else { 
			node* operand2 = operandStack.top(); //get node for second operand
			operand2->parent = root;
			operandStack.pop();
			node* operand1 = operandStack.top(); //get node for first operand
			operand1->parent = root;
			operandStack.pop();
			root->left = operand1; //assign the operand to the left and right of the root operator
			root->right = operand2;
		}
	}
	operandStack.push(root);
	return 0;
}

SyntaxTree::node* SyntaxTree::createNode(const string& s) 
//create node from string input and fill attributes according to type
{
	node* root = new node;

	if (isOperator(s) && !isQuantor(s)) { //for operators no arguments are needed
		root->type = s;
	}
	else { //for quantors and predicates arguments are needed
		const char* t = s.c_str();
		int index = 0;
		while (t[index++] != '(') {};
		string name = string(t, 0, index - 1);
		if (name == "Mod" || name == "TPO1" || name == "TPO2" || isQuantor(name))
			root->type = name;
		else {
			delete root;
			return nullptr;
		}

		vector<string> arguments;
		int argcount = 0;
		for (auto i = index; i < s.length(); i++) {
			if (t[i] == ',' || t[i] == ')') {
				string temp(t, index, i - index);
				index = i + 1;
				if (isValidArg(temp, argcount, name)) {
					root->args[argcount] = temp;
					argcount++;
				}
				else {
					delete root;
					return nullptr;
				}
			}
		}
	}
	return root;
}

void SyntaxTree::deleteBranch(node* root) 
//recursive deletion of each node in subtree
{
	if (root == nullptr)
		return;

	deleteBranch(root->left);
	deleteBranch(root->right);
	delete root;
}

bool SyntaxTree::isOperator(const string& s) 
//check if input string represents an FOL operator
{
	return (s == "(" || s == ")" || s == "~" ||
		s == "&" || s == "|" || s == "=>" ||
		s == "<=>" || isQuantor(s));
}

bool SyntaxTree::isQuantor(const string& s) 
//check if input string represents an FOL quantor
{
	return (s[0] == 'A' || s[0] == 'E');
}

bool SyntaxTree::isValidArg(const string& arg, const int& argcount, const string& name) 
//check if input string arg as argument nr argcount is valid for node with type name
{
	if (arg[0] == 'c' || arg[0] == 'u' || arg[0] == 'v' || arg[0] == 'w'
		|| arg[0] == 'x' || arg[0] == 'y' || arg[0] == 'z') { //check for allowed variables and constants
		bool numeric_args = true;
		for (auto i = 1; i < arg.length(); i++) { //numbers after letters are allowed to increase possible variables to use
			if (!isdigit(arg[i])) {
				numeric_args = false;
			}
		}
		if (numeric_args && arg.length() <= 3) { //only two numbers are allowed to index variables

			if ((name == "Mod" || isQuantor(name)) && argcount < 1) //predicate "Mod" and quantors have only one argument
				return true;

			if ((name == "TPO1" || name == "TPO2") && argcount < 2) //predicates "TPO1" and "TPO2" have two arguments
				return true;
		}
	}
	return false; 
}

int SyntaxTree::operatorPrecedence(const string& op) 
//get operator precedences as rank integer from name
{
	/*if (op == "(" || op == ")")
	return 8;*/
	if (op == "~")
		return 7;
	if (op == "&")
		return 6;
	if (op == "|")
		return 5;
	if (op == "=>")
		return 4;
	if (op == "<=>")
		return 3;
	if (op == "A")
		return 2;
	if (op == "E")
		return 1;
	return 0;
}





