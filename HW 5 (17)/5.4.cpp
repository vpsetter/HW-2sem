#include <iostream>
#include <regex>
#include <string>
#include <iomanip>

int main()
{
	std::regex pattern_date(R"(\d{4}\.((((0[13578])|(1[02]))\.(([0-2]\d)|(3[01])))|(((0[469])|(11))\.(([0-2]\d)|(30)))|(02\.[0-2]\d)))");
	std::regex pattern_time(R"((([01]\d)|(2[0-3]))(:[0-5]\d){2})");

	std::string data = R"(jjjj 1991.02.23 22:69:01 fj;sls;; 1521.10.10 d;lwwwl 03:08:41 2500.15.14 16:06:34 2020.02.29 24:00:00 23:59:59 23.59.59)";

	std::cout << "Dates:\n";

	std::for_each(std::sregex_iterator(std::cbegin(data), std::cend(data), pattern_date), std::sregex_iterator(), [](const std::smatch& match)
		{
			std::cout <<  match[0] << '\n';
		});

	std::cout << "\nTimes:\n";

	std::for_each(std::sregex_iterator(std::cbegin(data), std::cend(data), pattern_time), std::sregex_iterator(), [](const std::smatch& match)
		{
			std::cout << match[0] << '\n';
		});

	return 0;
}