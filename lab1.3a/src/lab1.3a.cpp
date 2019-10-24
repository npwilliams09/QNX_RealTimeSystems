#include <iostream>
#include <stdlib.h>
#include <stdio.h>
using namespace std;


struct complex {
	int x;
	int y;
};

complex adder(complex a, complex b){
	complex res;
	res.x = a.x + b.x;
	res.y = a.y + b.y;
	return res;
}

int main() {
	complex one; complex two;

	one.x = 2;
	one.y = 4;

	two.x = 3;
	two.y = 8;

	complex three = adder(one,two);

	cout << three.x << endl;
	cout << three.y << endl;
}
