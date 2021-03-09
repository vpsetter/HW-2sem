#include <iostream>
#include <iterator>
#include <locale>
#include <string>
#include <map>

#include <boost/locale.hpp>

#include <Windows.h>

std::string convert_locale_to_utf(const std::string & string)
{
	boost::locale::generator generator;
	generator.locale_cache_enabled(true);

	std::locale locale = generator(boost::locale::util::get_system_locale());

	return boost::locale::conv::to_utf < char >(string, locale);
}

std::string convert_utf_to_locale(const std::string& string)
{
	boost::locale::generator generator;
	generator.locale_cache_enabled(true);

	std::locale locale = generator(boost::locale::util::get_system_locale());

	return boost::locale::conv::from_utf < char >(string, locale);
}

int main()
{
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);

	std::unordered_map<char32_t, std::u32string> ru_en;	

	ru_en[U'�'] = U"A";
	ru_en[U'�'] = U"B";
	ru_en[U'�'] = U"V";
	ru_en[U'�'] = U"G";
	ru_en[U'�'] = U"D";
	ru_en[U'�'] = U"E";
	ru_en[U'�'] = U"E";
	ru_en[U'�'] = U"Zh";
	ru_en[U'�'] = U"Z";
	ru_en[U'�'] = U"I";
	ru_en[U'�'] = U"I";
	ru_en[U'�'] = U"K";
	ru_en[U'�'] = U"L";
	ru_en[U'�'] = U"M";
	ru_en[U'�'] = U"N";
	ru_en[U'�'] = U"O";
	ru_en[U'�'] = U"P";
	ru_en[U'�'] = U"R";
	ru_en[U'�'] = U"S";
	ru_en[U'�'] = U"T";
	ru_en[U'�'] = U"U";
	ru_en[U'�'] = U"F";
	ru_en[U'�'] = U"Kh";
	ru_en[U'�'] = U"Ts";
	ru_en[U'�'] = U"Ch";
	ru_en[U'�'] = U"Sh";
	ru_en[U'�'] = U"Shch";
	ru_en[U'�'] = U"''";
	ru_en[U'�'] = U"Y";
	ru_en[U'�'] = U"'";
	ru_en[U'�'] = U"E";
	ru_en[U'�'] = U"Yu";
	ru_en[U'�'] = U"Ya";

	ru_en[U'�'] = U"a";
	ru_en[U'�'] = U"b";
	ru_en[U'�'] = U"v";
	ru_en[U'�'] = U"g";
	ru_en[U'�'] = U"d";
	ru_en[U'�'] = U"e";
	ru_en[U'�'] = U"e";
	ru_en[U'�'] = U"zh";
	ru_en[U'�'] = U"z";
	ru_en[U'�'] = U"i";
	ru_en[U'�'] = U"i";
	ru_en[U'�'] = U"k";
	ru_en[U'�'] = U"l";
	ru_en[U'�'] = U"m";
	ru_en[U'�'] = U"n";
	ru_en[U'�'] = U"o";
	ru_en[U'�'] = U"p";
	ru_en[U'�'] = U"r";
	ru_en[U'�'] = U"s";
	ru_en[U'�'] = U"t";
	ru_en[U'�'] = U"u";
	ru_en[U'�'] = U"f";
	ru_en[U'�'] = U"kh";
	ru_en[U'�'] = U"ts";
	ru_en[U'�'] = U"ch";
	ru_en[U'�'] = U"sh";
	ru_en[U'�'] = U"shch";
	ru_en[U'�'] = U"''";
	ru_en[U'�'] = U"y";
	ru_en[U'�'] = U"'";
	ru_en[U'�'] = U"e";
	ru_en[U'�'] = U"yu";
	ru_en[U'�'] = U"ya";

	std::string string;
	std::u32string u32string_ru;
	std::u32string u32string_en;

	while (std::getline(std::cin, string))
	{
		string = convert_locale_to_utf(string);
		u32string_ru = boost::locale::conv::utf_to_utf < char32_t, char >(string);
		u32string_en.clear();

		std::for_each(std::begin(u32string_ru), std::end(u32string_ru),
			[&u32string_en, &ru_en](auto symbol)
			{if (symbol >= U'�' && symbol <= U'�')
		{
			std::copy(std::begin(ru_en[symbol]), std::end(ru_en[symbol]), std::back_inserter(u32string_en));
		}
			else
		{
			u32string_en.push_back(symbol);
		}
			});

		string = boost::locale::conv::utf_to_utf < char, char32_t >(u32string_en);

		string = convert_utf_to_locale(string);

		std::cout << string << "\n\n" ;
	}

	return 0;
}