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

	ru_en[U'À'] = U"A";
	ru_en[U'Á'] = U"B";
	ru_en[U'Â'] = U"V";
	ru_en[U'Ã'] = U"G";
	ru_en[U'Ä'] = U"D";
	ru_en[U'Å'] = U"E";
	ru_en[U'¨'] = U"E";
	ru_en[U'Æ'] = U"Zh";
	ru_en[U'Ç'] = U"Z";
	ru_en[U'È'] = U"I";
	ru_en[U'É'] = U"I";
	ru_en[U'Ê'] = U"K";
	ru_en[U'Ë'] = U"L";
	ru_en[U'Ì'] = U"M";
	ru_en[U'Í'] = U"N";
	ru_en[U'Î'] = U"O";
	ru_en[U'Ï'] = U"P";
	ru_en[U'Ð'] = U"R";
	ru_en[U'Ñ'] = U"S";
	ru_en[U'Ò'] = U"T";
	ru_en[U'Ó'] = U"U";
	ru_en[U'Ô'] = U"F";
	ru_en[U'Õ'] = U"Kh";
	ru_en[U'Ö'] = U"Ts";
	ru_en[U'×'] = U"Ch";
	ru_en[U'Ø'] = U"Sh";
	ru_en[U'Ù'] = U"Shch";
	ru_en[U'Ú'] = U"''";
	ru_en[U'Û'] = U"Y";
	ru_en[U'Ü'] = U"'";
	ru_en[U'Ý'] = U"E";
	ru_en[U'Þ'] = U"Yu";
	ru_en[U'ß'] = U"Ya";

	ru_en[U'à'] = U"a";
	ru_en[U'á'] = U"b";
	ru_en[U'â'] = U"v";
	ru_en[U'ã'] = U"g";
	ru_en[U'ä'] = U"d";
	ru_en[U'å'] = U"e";
	ru_en[U'¸'] = U"e";
	ru_en[U'æ'] = U"zh";
	ru_en[U'ç'] = U"z";
	ru_en[U'è'] = U"i";
	ru_en[U'é'] = U"i";
	ru_en[U'ê'] = U"k";
	ru_en[U'ë'] = U"l";
	ru_en[U'ì'] = U"m";
	ru_en[U'í'] = U"n";
	ru_en[U'î'] = U"o";
	ru_en[U'ï'] = U"p";
	ru_en[U'ð'] = U"r";
	ru_en[U'ñ'] = U"s";
	ru_en[U'ò'] = U"t";
	ru_en[U'ó'] = U"u";
	ru_en[U'ô'] = U"f";
	ru_en[U'õ'] = U"kh";
	ru_en[U'ö'] = U"ts";
	ru_en[U'÷'] = U"ch";
	ru_en[U'ø'] = U"sh";
	ru_en[U'ù'] = U"shch";
	ru_en[U'ú'] = U"''";
	ru_en[U'û'] = U"y";
	ru_en[U'ü'] = U"'";
	ru_en[U'ý'] = U"e";
	ru_en[U'þ'] = U"yu";
	ru_en[U'ÿ'] = U"ya";

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
			{if (symbol >= U'À' && symbol <= U'ÿ')
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