#include <iostream>
#include <regex>
#include <string>
#include <iomanip>

int main()
{
	std::regex pattern_mail(R"(^[[:alnum:]][-\.\w]*@(([[:alpha:]]+\.)+[[:alpha:]]{2,3}))");

	std::string str, full_str;

	while (std::getline(std::cin, str))
	{
		std::copy(std::cbegin(str), std::cend(str), std::back_inserter(full_str));
		full_str.push_back('\n');
	}

	std::cout << '\n' << std::left << std::setw(35) << std::setfill(' ') << "E-mail"
		<< "Domain" << "\n\n";
	std::for_each(std::sregex_iterator(std::cbegin(full_str), std::cend(full_str), pattern_mail), std::sregex_iterator(), [](const std::smatch& match)
		{
			std::cout << std::left << std::setw(35) << std::setfill(' ') << match[0]
				<< match[1] << '\n';
		});

	return 0;
}
