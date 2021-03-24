#include <algorithm>
#include <future>
#include <iostream>
#include <numeric>
#include <thread>
#include <vector>

template < typename Iterator, typename F >
void parallel_for_each(Iterator first, Iterator last, F function, const std::size_t max_size)
{
	const std::size_t length = std::distance(first, last);

	if (length <= max_size)
	{
		std::for_each(first, last, function);
	}
	else
	{
		Iterator middle = first;
		std::advance(middle, length / 2);

		std::future < void > first_half_result = std::async(parallel_for_each < Iterator, F >, first, middle, function, max_size);

		parallel_for_each(middle, last, function, max_size);
		first_half_result.get();
	}
}

int main()
{
	std::vector < int > v(10'000'000, 0);

	auto increment = [](int& x) { ++x; };

	const std::size_t max_size = v.size() / std::thread::hardware_concurrency();

	parallel_for_each(std::begin(v), std::end(v), increment, max_size);

	int errors = 0;

	auto test = [&errors](int x) {if (x != 1) { ++errors; }; };

	parallel_for_each(std::begin(v), std::end(v), test, max_size);

	std::cout << "Parallel for_each errors = " << errors << '\n';

	std::cout << (errors ? "Failure\n" : "Success\n");

	return 0;
}

