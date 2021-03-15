#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>

int main()
{
	const int lines = 101U;

	std::ofstream fout("file 6.2.txt", std::ios::out | std::ios::trunc);

	for (auto i = 0; i < lines; ++i)
	{
		fout << "String" << std::left << std::setw(5) << std::setfill(' ') << i << '\n';
	}

	fout.close();

	const int string_to_find = 56U; // 0-100

	std::ifstream fin("file 6.2.txt", std::ios::in);

	int first = fin.tellg(); 
	fin.seekg(-1, std::ios::end);
	int last = fin.tellg();

	auto str_lenght = (last + 1 - first) / lines;

	fin.seekg( (-1) * (lines - string_to_find) * str_lenght, std::ios::end);

	std::string string;

	std::getline(fin, string);

	fin.close();

	std::cout << "required string: " << string_to_find << "\nresult: " << string << '\n';

	return 0;
}