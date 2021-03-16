#include <filesystem>
#include <iomanip>
#include <istream>
#include <locale>
#include <ostream>
#include <iostream>
#include <string>
#include <fstream>

#include "json.hpp"

using nlohmann::json;

struct Person
{
	std::string first_name;
	std::string last_name;
	std::size_t year_of_birth;
	bool is_married;
};

std::istream& operator>>(std::istream& stream, Person& person)
{
	std::cin >> person.first_name >> person.last_name >> person.year_of_birth >> std::boolalpha >> person.is_married;
	return stream;
}

int main()
{
	system("chcp 65001");

	std::vector<Person> persons;

	Person person_read;
	while (std::cin >> person_read)
	{
		persons.push_back(person_read);
	}

	auto path = std::filesystem::current_path();
	std::filesystem::create_directory(path / "Persons");
	auto output_directory = path / "Persons";

	json j;

	for (auto& person : persons)
	{
		j["first_name"] = person.first_name;
		j["last_name"] = person.last_name;
		j["year_of_birth"] = person.year_of_birth;
		j["is_married"] = person.is_married;

		std::string filename = person.first_name + person.last_name + ".txt";

		auto file_path = output_directory / filename;

		std::ofstream fout(file_path.string(), std::ios::out | std::ios::trunc);

		fout << std::setw(4) << j;
	}

	return 0;	
}