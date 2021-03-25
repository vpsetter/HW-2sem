#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <chrono>
#include <execution>
#include <functional>

#include "Timer.hpp"

int main()
{
	const std::size_t size = 10'000'000U;

	std::vector <double> v(size, 0.0);
	std::vector <double> results(size, 1.0);
	std::iota(std::begin(v), std::end(v), 1.0);

	auto result1 = 0.0;
	auto result2 = 0.0;

	auto binary_op = [](double d1, double d2) { return std::sqrt(d1 * d1 + d2 * d2); };

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
		std::partial_sum(std::begin(v), std::end(v), std::begin(results), binary_op);
	}

	{
		Timer<std::chrono::microseconds> concurrent_time("concurrent partial sum (inclusive scan) time");
		std::inclusive_scan(std::execution::par, std::begin(v), std::end(v), std::begin(results), binary_op);
	}

	{
		Timer<std::chrono::microseconds> sequential_time("sequential inner product time");
		result1 = std::inner_product(std::begin(v), std::end(v), std::begin(results), 1.0);
	}

	{
		Timer<std::chrono::microseconds> concurrent_time("concurrent inner product (transform reduce) time");
		result2 = std::transform_reduce(std::execution::par, std::begin(v), std::end(v), std::begin(results), 1.0);
	}

	std::cout << "inner_product result = " << result1 << " , transform_reduce result = " << result2 << '\n';

	return 0;
}