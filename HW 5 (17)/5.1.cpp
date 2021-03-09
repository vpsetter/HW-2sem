#include <iostream>
#include <locale>
#include <iomanip>
#include <iterator>

#include <Windows.h> 

int main()
{
	SetConsoleCP(CP_UTF8);
	SetConsoleOutputCP(CP_UTF8);
	const double rubles_per_euro = 88.933;

	std::locale locale_in("deu_deu.utf8");
	std::locale locale_out("ru_RU.utf8");
	long double value = 0.0;
	std::cin.imbue(locale_in);
	while (std::cin >> std::get_money(value, true))
	{
		std::cout.imbue(locale_in);
		std::cout << std::showbase << std::put_money(value) << " <-> ";
		value *= rubles_per_euro;
		std::cout.imbue(locale_out);
		std::cout << std::showbase << std::put_money(value) << '\n';
	}
	
	return 0;
}