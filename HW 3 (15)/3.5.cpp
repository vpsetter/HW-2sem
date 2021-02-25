#include <iostream>
#include <string>
#include <array>
#include <iomanip>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/random_access_index.hpp>

using namespace boost::multi_index;

struct Person
{
	std::string last_name;
	long long phone;
};

std::ostream& operator<<(std::ostream& stream, const Person& person)
{
	std::cout << std::left << std::setw(25) << std::setfill(' ') << person.last_name << person.phone;
	return stream;
}

using persons_multi_index = multi_index_container<Person, indexed_by
	< 
	ordered_non_unique < member < Person, std::string, & Person::last_name > >,
	random_access <>,
	hashed_non_unique <	member < Person, long long, &Person::phone>>,
	hashed_non_unique <	member < Person, std::string, & Person::last_name>>
	>>;

int main()
{
	persons_multi_index reference_book;

	reference_book.insert({ "Ivanov"    , 9124636789 });
	reference_book.insert({ "Sidorov"   , 9821539882 });
	reference_book.insert({ "Petrov"    , 9124636789 });
	reference_book.insert({ "Petrov"    , 9861713319 });
	reference_book.insert({ "Stroustrup", 9887833439 });
	reference_book.insert({ "Petrova"   , 9861713319 });
	reference_book.insert({ "Stroustrup", 9318479012 });

	auto& ref_book_for_typography = reference_book.get < 0 >();
	auto& ref_book_for_advertisers = reference_book.get < 1 >();
	auto& ref_book_for_regular_user_by_phones = reference_book.get < 2 >();
	auto& ref_book_for_regular_user_by_last_names = reference_book.get < 3 >();

	std::cout << "Catalog for typography:\n";
	for (auto& person : ref_book_for_typography)
	{
		std::cout << person << '\n';
	}

	std::cout << "\nThe 3th person:\n" << *(std::next(reference_book.begin())) << '\n'; 
	std::cout << "\nSearching for the 3th person:\n" << ref_book_for_advertisers[2];
	
	std::cout << "\n\nFilter by phone 9861713319:\n";
	auto range1 = ref_book_for_regular_user_by_phones.equal_range(9861713319);
	for (auto it = range1.first; it != range1.second; ++it)
	{
		std::cout << *it << '\n';
	}

	std::cout << "\n\nFilter by last name Petrov:\n";
	auto range2 = ref_book_for_regular_user_by_last_names.equal_range("Petrov");
	for (auto it = range2.first; it != range2.second; ++it)
	{
		std::cout << *it << '\n';
	}

	for (auto it = range2.first; it != range2.second; ++it) //deleting {"Petrov", 9124636789}
	{
		if ((it->phone) == 9124636789)
		{
			ref_book_for_regular_user_by_last_names.erase(it);
			break;
		}
	}

	auto insert_it = ref_book_for_regular_user_by_phones.insert({ "Kudryavtsev", 4954085700 });
	std::cout << "\nadded:\n" << *insert_it.first << '\n';
	ref_book_for_regular_user_by_last_names.erase("Ivanov");
	std::cout << "\ndeleted Ivanov\n";
	ref_book_for_regular_user_by_last_names.modify(ref_book_for_regular_user_by_last_names.find("Petrova"),
			[](Person& person) {person.last_name = "Kudryavtseva"; }); //marriage
	std::cout << "Petrova -> Kudryavtseva\n";

	std::cout << "\nCatalog for typography:\n";
	for (auto& person : ref_book_for_typography)
	{
		std::cout << person << '\n';
	}

	return 0;
}

