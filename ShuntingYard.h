#include <stack>
#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include "Token.h"

const int precedence[5][3] = {
	{'^', 4, 0},
	{'*', 3, 1},
	{'/', 3, 1},
	{'+', 2, 1},
	{'-', 2, 1}
};

void copyStack(std::stack<Token> & s, std::stack<Token> & dest) {
	if(s.empty()) return;
	Token t = s.top();
	s.pop();
	copyStack(s, dest);
	dest.push(t);
	return;
}

class Shuntingyard {
public:
	char * _string;
	
	Shuntingyard(char * s) {
		_string = strdup(s);
	}
	
	~Shuntingyard() {
		delete _string;
	}
	
	
	
	std::vector<Token> process() {
		std::stack<Token> yard;
		std::vector<Token> ret;
		char * pstr = _string;
		while(*pstr) {
			if(*pstr == ' ' || *pstr == '\n' || *pstr == '\t') {
				pstr++;
				continue;
			} else if(*pstr >= '0' && *pstr <= '9') {
				char * n = (char *)malloc(sizeof(char) * strlen(_string) + 1);
				char * p = n;
				while(*pstr >= '0' && *pstr <= '9') {
					*p++ = *pstr++;
				}
				*p = 0;
				int inte = atoi(n);
				free(n);
				Token t(inte);
				ret.push_back(t);
			} else if(*pstr == '(') {
				Token t(*pstr++);
				yard.push(t);
			} else if(*pstr == ')') {
				while(!yard.empty()) {
					Token t = yard.top();
					yard.pop();
					if(t._op == '(') break;
					ret.push_back(t);
				}
				pstr++;
			} else if(*pstr == '^' || *pstr == '*' || *pstr == '/'
					|| *pstr == '+' || *pstr == '-') {
				//operator
				Token o1(*pstr++);
				while(!yard.empty()) {
					Token o2 = yard.top();
					int i1 = 0, i2 = 0;
					if(o2._op == '(') {
						break;
					}
					for(int i = 0; i < 5; i++) {
						if(precedence[i][0] == o1._op) i1 = i;
						if(precedence[i][0] == o2._op) i2 = i;
					}
					if(precedence[i1][2]) {
						//left-associative
						if(precedence[i1][1] <= precedence[i2][1]) {
							ret.push_back(o2);
							yard.pop();
						} else {
							break;
						}
					} else {
						//right associative
						if(precedence[i1][1] < precedence[i2][1]) {
							ret.push_back(o2);
							yard.pop();
						} else {
							break;
						}
					}
				}
				yard.push(o1);
			} else {
				//identifier
				Token t(*pstr++);
				ret.push_back(t);
			}
			/*for(int i = 0; i < ret.size(); i++) {
				ret[i].print();
			}
			fprintf(stderr, "\t\t");
			std::stack<Token> pyard = yard;
			while(!pyard.empty()) {
				Token t = pyard.top();
				pyard.pop();
				t.print();
			}
			fprintf(stderr, "\n");*/
		}
		while(!yard.empty()) {
			Token t = yard.top();
			yard.pop();
			if(t._op == ')') {
				fprintf(stderr, "mismatched parentheses\n");
				exit(1);
			} else {
				ret.push_back(t);
			}
		}
		return ret;
	}
};
