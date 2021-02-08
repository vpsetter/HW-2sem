#include <iostream>

void k() {}

void g() {}

void h() {}

int error()
{
	return -1;
}

int f(int a, int b)
{
	return (a + b) * (a + b);
}

double average(double a, double b)
{
	return (a + b) / 2;
}

int main()
{
	std::cout << "Hello, git!\n";
	std::cout << "Hello, chief!\n";

	std::cout << f(14, 11) << "\n";
	std::cout << average(1.0, 19.0) << "\n";
	std::cout << error() << "\n";

	return 0;
}