#include <iostream>

int error()
{
	return -1;
}

int f(int a, int b)
{
	return (a + b) * (a + b);
}

int main()
{
	std::cout << "Hello, git!\n";
	std::cout << "Hello, chief!\n";

	std::cout << f(14, 11) << "\n";

	return 0;
}
