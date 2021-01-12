#pragma once

#include <vector>
#include <stack>
#include <string>
#include <algorithm>

#include <iostream> //debugging

using namespace std;
	
class SyntaxTree {

	public:

		struct node {
			node* parent, * left, * right;
			string type;
			vector<string> args;

			node() : parent(nullptr), left(nullptr), right(nullptr), type(""), args(2, "") {}
		};

		node* root;
		
		SyntaxTree() {
			this->root = nullptr;
		}

		~SyntaxTree() {
			deleteBranch(this->root);
		}

		int parse_FOBC(const string &s);

		static bool isQuantor(const string& s);

		//output syntax tree to system console (debugging)
		static void createExpressionString(const node* root, const int& n) {
			if (root == nullptr)
				return;

			//cout << "Ebene " << n;
			for (auto i = 0; i < n; i++)
				cout << "\t";

			if (root->type == "~" || isQuantor(root->type)) {
				cout << root->type << " with Argument " <<root->args[0] << endl;
				createExpressionString(root->left, (n + 1));
			}
			else if (!isOperator(root->type)) {
				cout << root->type << " with Arguments " << root->args[0] << " and " << root->args[1] << endl; 
			}
			else {
				cout << "L-Branch of " << root->type << endl;
				createExpressionString(root->left, (n + 1));
				//cout << "Ebene " << n;
				for (auto i = 0; i < n; i++)
					cout << "\t";
				cout << "R-Branch of " << root->type << endl;
				createExpressionString(root->right, (n + 1));
			}
			return;
		}
		

	private:

		static node* createNode(const string& s);

		static bool isOperator(const string& s);

		static bool isValidArg(const string& arg, const int& argcount, const string& name);

		static int operatorPrecedence(const string& op);

		static node* createSyntaxTree(const vector<string>& expression);

		static vector<string> tokenize(const string& s);

		static void deleteBranch(node* root);

		static int addToStack(const string& op, stack<node*>& operandStack);

	};