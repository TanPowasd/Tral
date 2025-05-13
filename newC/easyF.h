#pragma once
#include"base.h"
#include"Mymath.h"
using ll = long long int;
using BIH = BigIntHex;
using namespace std;
BIH BigRead(ll _L) {
	BigIntHex big_read_use;
	string _num_to_read;
	cin >> _num_to_read;
	big_read_use.from_str(_num_to_read, _L);
	return big_read_use;
}
ll IntRead() {
	char ch;
	ll x = 0, f = 1;
	ch = getchar();
	while (ch == '-') {
		f = -f;
		ch = getchar();
	}
	while (ch >= '0' && ch <= '9') {
		x = x * 10 + ch - '0';
		ch = getchar();
	}
	return x * f;
}
void BigWrite(BIH output_,ll _L) {
	cout << output_.to_str(_L);
}
BIH QuickModPower(BIH _base, BIH _power, BIH _Mod) {
	BIH _result;
	_result.set(1);
	while (_power > BIH(0)) {
		if (_power % BIH(2) == BIH(1)) {
			_result = _result * _base % _Mod;
		}
		_base = _base * _base % _Mod;
		_power /= BIH(2);
	}
	return _result % _Mod;
}
BIH QuickPower(BIH _base, BIH _power) {
	BIH _result;
	_result.set(1);
	while (_power > BIH(0)) {
		if (_power % BIH(2) == BIH(1)) {
			_result = _result * _base;
		}
		_base = _base * _base;
		_power /= BIH(2);
	}
	return _result;
}