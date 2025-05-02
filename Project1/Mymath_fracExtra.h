#pragma once
#include"base.h"
using namespace std;
using ll = long long int;
using BIH = BigIntHex;
class Frac
{
public:
	void GetFrac(string _input) {
        size_t dotPos = _input.find('.');
        if (dotPos == string::npos) {
            // 没有小数点的情况
            Numerator = stoi(_input);
            Denominator = BIH(1);
        }
        else {
            string integerPart = _input.substr(0, dotPos);
            string fractionalPart = _input.substr(dotPos + 1);
            string numStr = integerPart + fractionalPart;

            // 处理前导零（如空整数部分的情况）
            if (numStr.empty()) {
                Numerator = BIH(0);
                Denominator = BIH(1);
            }
            else {
                Numerator = stoi(numStr);
                Denominator = 1;
                for (size_t i = 0; i < fractionalPart.length(); ++i) {
                    Denominator *= BIH(10);
                }
            }
        }
	}
    void Simplify() {
        
    }
	void output() {
		cout << Numerator.to_str() << " " << Denominator.to_str();
	}
private:
	BIH Denominator=BIH(1);//分母
	BIH Numerator=BIH(0);//分子
};