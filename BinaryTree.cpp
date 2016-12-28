#include "BinaryTree.h"

BinaryTree::BinaryTree() {
	_root = NULL;
}

//helper to free the tree
//traverse the tree in post_order
void freeTree(TreeNode * root) {
	if(root == NULL) return;
	freeTree(root -> _left);
	freeTree(root -> _right);
	delete root;
}

BinaryTree::~BinaryTree() {
	//freeTree(_root);
}

void printTree(TreeNode * root) {
	if(root == NULL) return;
	if(root -> t -> elementType == operation) {
		putchar('(');
		fflush(stdout);
	}
	printTree(root -> _left);
	root -> t -> print();
	printTree(root -> _right);
	if(root -> t -> elementType == operation) {
		putchar(')');
		fflush(stdout);
	}
}

void
BinaryTree::print() {
	printTree(_root);
	putchar('\n');
}
