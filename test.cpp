#include "ExpressionApplication.h"
#include "ShuntingYard.h"

int main() {
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
	printf("Original Tree:\t\t");
	fflush(stdout);
	e._expression -> print();
	printf("Differentiated Tree:\t");
	fflush(stdout);
	BinaryTree * ret = e.differentiate('x');
	ret -> print();
	printf("Simplified Tree:\t");
	fflush(stdout);
	ret = e.simplify();
	ret -> print();
}