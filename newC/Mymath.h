#pragma once
#include"base.h"
using namespace std;
using ll = long long int;
using BIH = BigIntHex;
//实现三角函数计算和基本计算器能力
stack< BIH >numq;
stack< char >charq;
int _firstFlag = 0;
int _firstFlag2 = 0;
int killFlag = 0;//用来判断除数是0的情况
void calculate(stack<BIH>& numq, char s_char) {
	BIH x_calcuate, y_calcuate, z_calcuate;
	switch (s_char) {
	case '+':
	{
		x_calcuate = numq.top();
		numq.pop();
		y_calcuate = numq.top();
		numq.pop();
		z_calcuate = x_calcuate + y_calcuate;
		numq.push(z_calcuate);
		break;
	}
	case '-':
	{
		x_calcuate = numq.top();
		numq.pop();
		y_calcuate = numq.top();
		numq.pop();
		z_calcuate = x_calcuate - y_calcuate;
		numq.push(z_calcuate);
		break;
	}
	case '*':
	{
		x_calcuate = numq.top();
		numq.pop();
		y_calcuate = numq.top();
		numq.pop();
		z_calcuate = x_calcuate * y_calcuate;
		numq.push(z_calcuate);
		break;
	}
	case '/':
	{
		x_calcuate = numq.top();
		numq.pop();
		y_calcuate = numq.top();
		numq.pop();
		if (x_calcuate == BIH(0)) {
			killFlag = 1;
			break;
		}
		z_calcuate = y_calcuate / x_calcuate;
		numq.push(z_calcuate);
	}
	}
}
int fuhao(char s_fuhao) {
	if (s_fuhao == '+' || s_fuhao == '-') {
		return 1;
	}
	if (s_fuhao == '*' || s_fuhao == '/') {
		return 2;
	}
	if (s_fuhao == '(' || s_fuhao == ')') {
		return 3;
	}
	if (s_fuhao == '#') {
		return -1;
	}
}
BIH MATHfunctionSolve(char a_function[10001]) {
	charq.push('#');
	int lena_function = strlen(a_function);
	if (a_function[0] == '-') {
		_firstFlag = 1;
		a_function[0] = '0';
	}
	for (int i = 0; i < lena_function; i++) {
		//主循环
		if (a_function[i] == '(') {
			charq.push(a_function[i]);
		}
		else if (a_function[i] == ')') {
			while (charq.top() != '(') {
				calculate(numq, charq.top());
				charq.pop();
			}
			charq.pop();
		}
		else if (a_function[i] >= '0' && a_function[i] <= '9') {
			BIH numtemp = BIH(0);
			while (a_function[i] >= '0' && a_function[i] <= '9') {
				numtemp *= BIH(10);
				numtemp += BIH(int(a_function[i] - '0'));
				i++;
			}
			i--;
			if (_firstFlag2 == 1 && _firstFlag == 0) {
				numtemp *= BIH(-1);
			}
			numq.push(numtemp);
			_firstFlag = 1;
		}
		else {
			if (a_function[i] == '+' || a_function[i] == '-') {
				if ((fuhao(charq.top()) >= fuhao(a_function[i])) && charq.top() != '(') {
					while (fuhao(charq.top()) >= fuhao(a_function[i]) && charq.top() != '(') {
						calculate(numq, charq.top());
						charq.pop();
					}
					charq.push(a_function[i]);
				}
				else {
					charq.push(a_function[i]);
				}
			}
			else if (a_function[i] == '*' || a_function[i] == '/') {
				if (fuhao(charq.top()) == fuhao(a_function[i])) {
					calculate(numq, charq.top());
					if (killFlag == 1) {
						printf("Error");
						throw "Error";
					}
					charq.pop();
					charq.push(a_function[i]);
				}
				else {
					charq.push(a_function[i]);
				}
			}
		}
	}
	while (charq.top() != '#') {
		calculate(numq, charq.top());
		if (killFlag == 1) {
			printf("Error");
			throw "Error";
		}
		charq.pop();
	}
	//cout << numq.top().to_str();
	return numq.top();
}
BIH bigrand() {
	BIH nowtime=BIH(time(0));
	return nowtime;
}