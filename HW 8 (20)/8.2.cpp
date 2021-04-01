#include <string>
#include <iterator>
#include <thread>
#include <future>
#include <algorithm>
#include <iostream>
#include <iomanip>

std::mutex mutex;

template<typename T>
void safe_push_back(std::vector<T>& vector, T value)
{
	std::lock_guard < std::mutex > lock(mutex);
	vector.push_back(value);
}

struct find_substr_block
{
	void operator()(const std::string & string_to_find, const std::string & string,
		std::size_t first, std::size_t last, std::vector<std::size_t>& indices)
	{
		auto length = string_to_find.size();
		auto begin = std::next(std::begin(string), first);
		auto end = std::next(begin, length - 1);
		
		for (auto index = first; index < last; ++index)
		{
			if (string_to_find == std::string(begin++, ++end))
			{
				safe_push_back(indices, index);
			}
		}
	}
};

void parallel_find_substr(const std::string & string_to_find, const std::string & string, std::size_t first, std::size_t last, std::vector<std::size_t>& indices)
{
	auto substring_length = string_to_find.size();
	auto string_length = string.size();

	const std::size_t hardware_threads = std::thread::hardware_concurrency();

	if ((last - first) <= std::max(16U, (string_length - substring_length + 1) / hardware_threads ))
	{
		find_substr_block()(string_to_find, string, first, last, indices);
	}
	else
	{
		auto middle = first + (last - first) / 2;

		std::future < void > first_half = std::async(parallel_find_substr, std::cref(string_to_find), std::cref(string), first, middle, std::ref(indices));

		find_substr_block()(string_to_find, string, middle, last, indices);

		first_half.get();
	}
}

int main()
{
	const std::string string = "AGCTTTTGCTAAATGCGGCTTAGGTGTCAGTGGTTTCGAGTCGTGGGTTTAAAATGTGTCCCGTTGAAAGTGGGGCCTTGAAATTCGTAGT";
	std::cout << "String: " << string << '\n' << "Enter a substring\n";
	std::string substring = "";
	std::cin >> substring;

	std::vector <std::size_t> indices;

	parallel_find_substr(substring, string, 0U, string.size() - substring.size() + 1, indices);

	std::sort(std::begin(indices), std::end(indices));

	std::cout << "Indices of the first symbols:\n";

	for (auto index : indices)
	{
		std::cout << index << '\n';
	}

	return 0;
}