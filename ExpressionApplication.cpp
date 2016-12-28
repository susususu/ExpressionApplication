#include "ExpressionApplication.h"

using namespace std;

BinaryTree *
ExpressionApplication::buildExpressionTree(vector<Token> &v) {
	stack<TreeNode *> s;
	BinaryTree * tree = new BinaryTree;
	
	for(int i = 0; i < v.size(); i++) {
		if( v[i].elementType == integer
		|| v[i].elementType == identifier ) {
			TreeNode * t = new TreeNode(v[i]);
			s.push(t);
		} else {
			TreeNode * t1 = s.top(); s.pop();
			TreeNode * t2 = s.top(); s.pop();
			TreeNode * t = new TreeNode(v[i]);
			tree -> _root = t;
			t -> _left = t2;
			t -> _right = t1;
			t2 -> _parent = t;
			t1 -> _parent = t;
			s.push(t);
		}
	}
	if(!s.empty()) {
		TreeNode * t = s.top();
		tree -> _root = t;
	}
	return tree;
}

ExpressionApplication::ExpressionApplication(vector<Token> &v) {
	_expression = buildExpressionTree(v);
}

ExpressionApplication::~ExpressionApplication() {
	//if(_expression) delete _expression;
}

TreeNode * treedup(TreeNode * root) {
	if(root == 0) return NULL;
	TreeNode * t = new TreeNode(*(root -> t));
	t -> _left = treedup(root -> _left);
	t -> _right = treedup(root -> _right);
	return t;
}

TreeNode * diffTree(TreeNode * root, int isPow, char target) {
	if(root == NULL) return NULL;
	if(root -> t -> elementType == integer) {
		//in case of a constant for power
		//return number - 1
		if(isPow) {
			root -> t -> _num -= 1;
			return root;
		}
		//integer constant
		//delete the node and return NULL
		root -> t -> _num = 0;
		return root;
	} else if(root -> t -> elementType == identifier) {
		if(root -> t -> _id == target) {
			//identifier
			//return the identifier if is a power expression		
			if(isPow) return root;
			root -> t -> elementType = integer;
			root -> t -> _num = 1;
			return root;
		} else {
			//identifier is not the target
			//return 0 node
			root -> t -> elementType = integer;
			root -> t -> _num = 0;
			return root;
		}
	}
	
	if(root -> t -> _op == '^') {
		//power rule
		root -> _left = diffTree(root -> _left, 1, target);
		root -> _right = diffTree(root -> _right, 1, target); 
		Token t1('*');
		Token t2(root -> _right -> t -> _num + 1);
		TreeNode * newroot = new TreeNode(t1);
		newroot -> _left = new TreeNode(t2);
		newroot -> _right = root;
		root = newroot;
	} else if(root -> t -> _op == '+' || root -> t -> _op == '-') {
		//Sum rule
		root -> _left = diffTree(root -> _left, 0, target);
		root -> _right = diffTree(root -> _right, 0, target);
	} else if(root -> t -> _op == '*') {
		//Product Rule
		//f`(xy) = (f`x)y + x(f`y)
		//save the original child
		TreeNode * oleft = treedup(root -> _left);
		TreeNode * oright = treedup(root -> _right);
		
		TreeNode * lnode = diffTree(root -> _left, 0, target);
		TreeNode * rnode = diffTree(root -> _right, 0, target);
		root -> t -> _op = '+';
		Token plus('+');
		Token mult('*');
		TreeNode * newleft = new TreeNode(mult);
		newleft -> _left = lnode;
		newleft -> _right = oright;
		TreeNode * newright = new TreeNode(mult);
		newright -> _left = oleft;
		newright -> _right = rnode;
		root -> _left = newleft;
		root -> _right = newright;
	} else {
		//Quotient Rule
		TreeNode * oleft = treedup(root -> _left);
		TreeNode * oright = treedup(root -> _right);
		
		TreeNode * lnode = diffTree(root -> _left, 0, target);
		TreeNode * rnode = diffTree(root -> _right, 0, target);
		Token minu('-');
		Token mult('*');
		Token power('^');
		TreeNode * numerator = new TreeNode(minu);
		TreeNode * nleft = new TreeNode(mult);
		TreeNode * nright = new TreeNode(mult);
		nleft -> _left = lnode;
		nleft -> _right = oright;
		nright -> _left = oleft;
		nright -> _right = rnode;
		numerator -> _left = nleft;
		numerator -> _right = nright;
		TreeNode * denominator = new TreeNode(power);
		Token two(2);
		denominator -> _left = oright;
		denominator -> _right = new TreeNode(two);
		root -> _left = numerator;
		root -> _right = denominator;
	}
	return root;
}

TreeNode * simplifyTree(TreeNode * root) {
	if(root == NULL) return NULL;
	if(root -> _left == NULL && root -> _right == NULL) {
		return root;
	}
	TreeNode * lnode = simplifyTree(root -> _left);
	TreeNode * rnode = simplifyTree(root -> _right);
	if(root -> t -> _op == '+') {
		if(lnode -> t -> elementType == integer
		&& rnode -> t -> elementType == integer) {
			//constant expression
			int l = lnode -> t -> _num;
			int r = rnode -> t -> _num;
			int result = l + r;
			root -> t -> elementType = integer;
			root -> t -> _num = result;
			//delete children
			delete root -> _left;
			delete root -> _right;
			root -> _left = NULL;
			root -> _right = NULL;
			return root;
		} else if(lnode -> t -> elementType == identifier
			&& rnode -> t -> elementType == identifier) {
				//x + x
			if(lnode -> t -> _id == rnode -> t -> _id) {
				root -> t -> _op = '*';
				lnode -> t -> elementType = integer;
				lnode -> t -> _num = 2;
			}
		} else {
			if(lnode -> t -> elementType == integer
			&& lnode -> t -> _num == 0) {
				//0 + x
				//delete left children and root itself
				TreeNode * ret = root -> _right;
				delete root -> _left;
				delete root;
				root -> _left = NULL;
				root = ret;
				return root;
			} else if(rnode -> t -> elementType == integer
					&& rnode -> t -> _num == 0) {
				//x + 0
				//delete right children and root itself
				TreeNode * ret = root -> _left;
				delete root -> _right;
				delete root;
				root -> _right = NULL;
				root = ret;
				return root;
			}
		}
	} else if(root -> t -> _op == '-') {
		if(lnode -> t -> elementType == integer
		&& rnode -> t -> elementType == integer) {
			//constant expression
			int l = lnode -> t -> _num;
			int r = rnode -> t -> _num;
			int result = l - r;
			root -> t -> elementType = integer;
			root -> t -> _num = result;
			//delete children
			delete root -> _left;
			delete root -> _right;
			root -> _left = NULL;
			root -> _right = NULL;
			return root;
		} else if(rnode -> t -> elementType == integer
					&& rnode -> t -> _num == 0) {
			//x - 0
			//delete right children and root itself
			TreeNode * ret = root -> _left;
			delete root -> _right;
			delete root;
			root -> _right = NULL;
			root = ret;
			return root;
		}
	} else if(root -> t -> _op == '*') {
		if(lnode -> t -> elementType == integer
		&& rnode -> t -> elementType == integer) {
			//constant expression
			int l = lnode -> t -> _num;
			int r = rnode -> t -> _num;
			int result = l * r;
			root -> t -> elementType = integer;
			root -> t -> _num = result;
			//delete children
			delete root -> _left;
			delete root -> _right;
			root -> _left = NULL;
			root -> _right = NULL;
			return root;
		} else {
			if(lnode -> t -> elementType == integer) {
				if(lnode -> t -> _num == 0) {
					//0 * x
					root -> t -> elementType = integer;
					root -> t -> _num = 0;
					delete root -> _left;
					delete root -> _right;
					root -> _left = NULL;
					root -> _right = NULL;
					return root;
				} else if(lnode -> t -> _num == 1) {
					//1 * x
					TreeNode * ret = root -> _right;
					delete root -> _left;
					delete root;
					root -> _left = NULL;
					root = ret;
					return root;
				}
			} else if(rnode -> t -> elementType == integer) {
				if(rnode -> t -> _num == 0) {
					//x * 0
					root -> t -> elementType = integer;
					root -> t -> _num = 0;
					delete root -> _left;
					delete root -> _right;
					root -> _left = NULL;
					root -> _right = NULL;
					return root;
				} else if(rnode -> t -> _num == 1) {
					//x * 1
					TreeNode * ret = root -> _left;
					delete root -> _right;
					delete root;
					root -> _right = NULL;
					root = ret;
					return root;
				}
			}
		}
	} else if(root -> t -> _op == '/') {
		if(lnode -> t -> elementType == integer
		&& rnode -> t -> elementType == integer) {
			//constant expression
			int l = lnode -> t -> _num;
			int r = rnode -> t -> _num;
			int result = l / r;
			root -> t -> elementType = integer;
			root -> t -> _num = result;
			//delete children
			delete root -> _left;
			delete root -> _right;
			root -> _left = NULL;
			root -> _right = NULL;
			return root;
		} else {
			
		}
	} else if(root -> t -> _op == '^') {
		if(lnode -> t -> elementType == integer
		&& rnode -> t -> elementType == integer) {
			//constant expression
			int l = lnode -> t -> _num;
			int r = rnode -> t -> _num;
			int result = 1;
			for(int i = 0; i < r; i++) result *= l;
			
			//0 ^ n
			if(l == 0) result = 0;
			
			root -> t -> elementType = integer;
			root -> t -> _num = result;
			//delete children
			delete root -> _left;
			delete root -> _right;
			root -> _left = NULL;
			root -> _right = NULL;
			return root;
		} else {
			
		}
	}
	return root;
}

BinaryTree *
ExpressionApplication::differentiate(char c) {
	TreeNode * sav = treedup(_expression -> _root);
	TreeNode * t = simplifyTree(_expression -> _root);
	t = diffTree(t, 0, c);
	_expression -> _root = sav;
	BinaryTree * bt = new BinaryTree;
	bt -> _root = t;
	return bt;
}

BinaryTree *
ExpressionApplication::simplify() {
	BinaryTree * bt = new BinaryTree;
	TreeNode * sav = treedup(_expression -> _root);
	TreeNode * t = simplifyTree(_expression -> _root);
	t = diffTree(t, 0, 'x');
	t = simplifyTree(t);
	_expression -> _root = sav;
	bt -> _root = t;
	return bt;
}

//I keep this here to illustrate how the stuff works
/*int main() {
	char buf[1000];
	char * p = buf;
	int c;
	while((c = getchar()) != EOF) {
		*p++ = c;
	}
	*p = 0;
	Shuntingyard s(buf);
	std::vector<Token> v = s.process();
	
	ExpressionApplication e(v);
	BinaryTree * ret = e.simplify();
	ret -> print();
	e._expression -> print();
}*/
