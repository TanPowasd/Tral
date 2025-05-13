#pragma once
#include"base.h"
#include"Mymath.h"
#include"easyF.h"
using namespace std;
class Frac {
private:
	BIH numerator = BIH(1);
	BIH denominator = BIH(1);
	bool isf = false;
	inline BIH gcd(const BIH& a, const BIH& b) {
		if (b == BIH(0)) {
			return a;
		}
		return gcd(b, a % b);
	}
	inline void simplify() {
		BIH divisor = gcd(numerator, denominator);
		numerator /= divisor;
		denominator /= divisor;
		if (denominator < BIH(0)) {
			numerator *= BIH(-1);
			denominator *= BIH(-1);
		}
	}

public:
	inline void getNUM(BIH fir, BIH sec) {
		if (sec == BIH(0)) {
			cout << "Denominator cannot be zero!" << endl;
			throw invalid_argument("Denominator cannot be zero!");
		}
		numerator = fir;
		denominator = sec;
	}
	inline void from_string() {
		//输入字符串构造
		string s;
		cin >> s;
		if (s[0] == '-') {
			isf = true;
			s = s.substr(1);
		}
		regex int_regex(R"(^-?\d+$)");  // 匹配整数，包括负数
		regex float_regex(R"(^-?\d+\.\d+$)");  // 匹配浮点数，包括负数
		regex invalid_regex(R"(^.*[a-zA-Z]+.*$)");  // 匹配包含字母的无效字符
		if (regex_match(s, int_regex)) {
			numerator = BIH(s);
			denominator = BIH(1);
		}
		else if (regex_match(s, float_regex)) {
			size_t dotpos = s.find(".");
			string integerPart = s.substr(0, dotpos);
			string decimalPart = s.substr(dotpos + 1);
			ll L_decimalPart = decimalPart.length();
			ll L_integerPart = integerPart.length();
			BIH numeratorpart = BIH(integerPart) * BIH(QuickPower(BIH(10), BIH(L_decimalPart))) + BIH(decimalPart);
			BIH denominatorpart = BIH(QuickPower(BIH(10), BIH(L_decimalPart)));
			getNUM(numeratorpart, denominatorpart);

		}
		else if (regex_match(s, invalid_regex)) {
			cout << "Invalid input!" << endl;
			throw invalid_argument("Invalid input!");
		}
		else {
			cout << "Invalid input!" << endl;
			throw invalid_argument("Invalid input!");
		}
		if (isf == true) {
			numerator = -numerator;
		}
	}
	inline void Void_from_string(string s) {
		// 已知分数的字符串形式，构造分数类
		if (s[0] == '-') {
			isf = true;
			s = s.substr(1);
		}
		regex int_regex(R"(^-?\d+$)");  // 匹配整数，包括负数
		regex float_regex(R"(^-?\d+\.\d+$)");  // 匹配浮点数，包括负数
		regex invalid_regex(R"(^.*[a-zA-Z]+.*$)");  // 匹配包含字母的无效字符
		if (regex_match(s, int_regex)) {
			numerator = BIH(s);
			denominator = BIH(1);
		}
		else if (regex_match(s, float_regex)) {
			size_t dotpos = s.find(".");
			string integerPart = s.substr(0, dotpos);
			string decimalPart = s.substr(dotpos + 1);
			ll L_decimalPart = decimalPart.length();
			ll L_integerPart = integerPart.length();
			BIH numeratorpart = BIH(integerPart) * BIH(QuickPower(BIH(10), BIH(L_decimalPart))) + BIH(decimalPart);
			BIH denominatorpart = BIH(QuickPower(BIH(10), BIH(L_decimalPart)));
			getNUM(numeratorpart, denominatorpart);

		}
		else if (regex_match(s, invalid_regex)) {
			cout << "Invalid input!" << endl;
			throw invalid_argument("Invalid input!");
		}
		else {
			cout << "Invalid input!" << endl;
			throw invalid_argument("Invalid input!");
		}
		if (isf == true) {
			numerator = -numerator;
		}
	}
	inline void to_string() {
		if (isf == true) {
			cout << "-";
		}
		BigWrite(numerator, 10);
		cout << "/";
		BigWrite(denominator, 10);
	}
	inline void FracSpy() {
		//化简分数
		simplify();
	}
	Frac() = default;
	Frac(BIH num, BIH den) :numerator(num), denominator(den) {
		if (den == BIH(0)) {
			cout << "Denominator cannot be zero!" << endl;
			throw invalid_argument("Denominator cannot be zero!");
		}
		simplify();
	}
	explicit Frac(BIH num) :numerator(num), denominator(BIH(1)) {}
	Frac(const string& s) {
		Void_from_string(s);
	}
	Frac operator+(const Frac& rhs) const {
		BIH new_num = numerator * rhs.denominator + denominator * rhs.numerator;
		BIH new_den = denominator * rhs.denominator;
		return Frac(new_num, new_den);
	}
	Frac operator-(const Frac& rhs) const {
		BIH new_num = numerator * rhs.denominator - denominator * rhs.numerator;
		BIH new_den = denominator * rhs.denominator;
		return Frac(new_num, new_den);
	}
	Frac operator*(const Frac& rhs) const {
		BIH new_num = numerator * rhs.numerator;
		BIH new_den = denominator * rhs.denominator;
		return Frac(new_num, new_den);
	}
	Frac operator/(const Frac& rhs) const {
		if (rhs.numerator == BIH(0)) {
			cout << "Denominator cannot be zero!" << endl;
			throw invalid_argument("Denominator cannot be zero!");
		}
		return Frac(numerator * rhs.denominator, denominator * rhs.numerator);
	}
	friend bool operator==(const Frac& lhs, const Frac& rhs) {
		return lhs.numerator * rhs.denominator == rhs.numerator * lhs.denominator;
	}
	friend bool operator!=(const Frac& lhs, const Frac& rhs) {
		return !(lhs == rhs);
	}
	friend bool operator<(const Frac& lhs, const Frac& rhs) {
		return lhs.numerator * rhs.denominator < rhs.numerator * lhs.denominator;
	}
	friend bool operator>(const Frac& lhs, const Frac& rhs) { return rhs < lhs; }
	friend bool operator<=(const Frac& lhs, const Frac& rhs) { return !(lhs > rhs); }
	friend bool operator>=(const Frac& lhs, const Frac& rhs) { return !(lhs < rhs); }
	friend istream& operator>>(istream& is, Frac& f) {
		f.from_string();
		return is;
	}
	friend ostream& operator<<(ostream& os, Frac& f) {
		f.to_string();
		return os;
	}
};