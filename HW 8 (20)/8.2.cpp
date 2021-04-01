#include <string>
#include <iterator>
#include <thread>
#include <future>
#include <algorithm>
#include <iostream>
#include <iomanip>

template<typename T>
void safe_push_back(std::vector<T>& vector, T value, std::mutex& mutex)
{
	std::scoped_lock lock(mutex);
	vector.push_back(value);
}

struct find_substr_block
{
	void operator()(const std::string & string_to_find, const std::string & string,
		std::size_t first, std::size_t last, std::vector<std::size_t>& indices, std::mutex& mutex)
	{
		auto length = string_to_find.size();
		auto begin = std::begin(string);
		auto substr_begin = std::begin(string_to_find);
		auto substr_end = std::end(string_to_find);
		
		for (auto index = first; index < last; ++index)
		{
			auto it1 = substr_begin;
			auto it2 = std::next(begin, index);
			for (; it1 != substr_end && *it1 == *it2; ++it1, ++it2);
			if (it1 == substr_end)
			{
				safe_push_back(indices, index, mutex);
			}
		}
	}
};

void parallel_find_substr(const std::string & string_to_find, const std::string & string,
	std::size_t first, std::size_t last, std::vector<std::size_t>& indices, std::mutex& mutex)
{
	auto substring_length = string_to_find.size();
	auto string_length = string.size();

	const std::size_t hardware_threads = std::thread::hardware_concurrency();

	if ((last - first) <= std::max(16U, (string_length - substring_length + 1) / hardware_threads ))
	{
		find_substr_block()(string_to_find, string, first, last, indices, mutex);
	}
	else
	{
		auto middle = first + (last - first) / 2;

		std::future < void > first_half = std::async(parallel_find_substr, std::cref(string_to_find), std::cref(string),
			first, middle, std::ref(indices), std::ref(mutex));

		parallel_find_substr(string_to_find, string, middle, last, indices, mutex);

		first_half.get();
	}
}

int main()
{
	std::mutex mutex;

	const std::string string = "AGCTTTTGCTAAATGCGGCTTAGGTGTCAGTGGTTTCGAGTCGTGGGTTTAAAATGTGTCCCGTTGAAAGTGGGGCCTTGAAATTCGTAGT";
	std::cout << "String: " << string << '\n' << "Enter a substring\n";
	std::string substring = "";
	std::cin >> substring;

	std::vector <std::size_t> indices;

	parallel_find_substr(substring, string, 0U, string.size() - substring.size() + 1, indices, mutex);

	std::sort(std::begin(indices), std::end(indices));

	std::cout << "Indices of the first symbols:\n";

	for (auto index : indices)
	{
		std::cout << index << '\n';
	}

	return 0;
}