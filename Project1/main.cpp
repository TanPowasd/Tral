#include"easyF.h"
#include"MyGame.h"
#include"JustFun.h"
#include"Mymath_fracExtra.h"
using namespace std;
char a[1001];
int main() {
	string h;
	Frac f;
	cin >> h;
	f.GetFrac(h);
	f.Simplify();
	f.output();
}