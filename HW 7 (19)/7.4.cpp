#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <chrono>
#include <execution>

#include "Timer.hpp"

int main()
{
	const std::size_t size = 10'000'000U;

	std::vector <int> v(size, 0);
	std::vector <long long> results(size, 0ll);
	std::iota(std::begin(v), std::end(v), 0);

	std::cout << "size of vector: " << size << '\n';

	{
		Timer<std::chrono::microseconds> sequential_time("sequential sort time");
		std::sort(std::begin(v), std::end(v));
	}

	{
		Timer<std::chrono::microseconds> concurrent_time("concurrent sort time");
		std::sort(std::execution::par, std::begin(v), std::end(v));
	}

	{
		Timer<std::chrono::microseconds> sequential_time("sequential partial sum time");
		std::partial_sum(std::begin(v), std::end(v), std::begin(results));
	}

	{
		Timer<std::chrono::microseconds> concurrent_time("concurrent partial sum (inclusive scan) time");
		std::inclusive_scan(std::execution::par, std::begin(v), std::end(v), std::begin(results));
	}

	{
		Timer<std::chrono::microseconds> sequential_time("sequential inner product time");
		auto result = std::inner_product(std::begin(v), std::end(v), std::begin(results), 0);
	}

	{
		Timer<std::chrono::microseconds> concurrent_time("concurrent inner product (transform reduce) time");
		auto result = std::transform_reduce(std::execution::par, std::begin(v), std::end(v), std::begin(results), 0);
	}

	return 0;
}