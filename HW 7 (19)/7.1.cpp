#include <iostream>
#include <random>
#include <chrono>
#include <vector>
#include <numeric>
#include <iterator>
#include <thread>
#include <functional>
#include <future>
#include <algorithm>

#include "Timer.hpp"

class Threads_Guard
{
public:
	explicit Threads_Guard(std::vector < std::thread > & threads) : m_threads(threads)
	{}

	Threads_Guard(Threads_Guard const&) = delete;

	Threads_Guard& operator=(Threads_Guard const&) = delete;

	~Threads_Guard() noexcept
	{
		try
		{
			for (std::size_t i = 0; i < m_threads.size(); ++i)
			{
				if (m_threads[i].joinable())
				{
					m_threads[i].join();
				}
			}
		}
		catch (...)
		{
			std::cout << "\nDTOR THREAD ERROR\n";
		}
	}

private:
	std::vector < std::thread >& m_threads;
};

template < typename Iterator, typename F>
struct count_if_block
{
	std::size_t operator()(Iterator first, Iterator last, F predicate)
	{
		return std::count_if(first, last, predicate);
	}
};

template < typename Iterator, typename F >
std::size_t parallel_count_if(Iterator first, Iterator last, F predicate)
{
	const std::size_t length = std::distance(first, last);

	if (!length)
	{
		return 0U;
	}

	const std::size_t min_per_thread = 32U;
	const std::size_t max_threads =	(length + min_per_thread - 1U) / min_per_thread;

	const std::size_t hardware_threads = std::thread::hardware_concurrency();

	const std::size_t num_threads =	std::min(hardware_threads != 0 ? hardware_threads : 2, max_threads);

	const std::size_t block_size = length / num_threads;

	std::vector < std::future < std::size_t > > futures(num_threads - 1);
	std::vector < std::thread > threads(num_threads - 1);

	Threads_Guard guard(threads);

	Iterator block_start = first;

	for (std::size_t i = 0; i < (num_threads - 1); ++i)
	{
		Iterator block_end = block_start;
		std::advance(block_end, block_size);

		std::packaged_task < std::size_t(Iterator, Iterator, F) > task{count_if_block < Iterator, F >()};

		futures[i] = task.get_future();
		threads[i] = std::thread(std::move(task), block_start, block_end, predicate);
		block_start = block_end;
	}

	std::size_t last_result = count_if_block < Iterator, F >()(block_start, last, predicate);

	std::size_t result = 0U;

	for (std::size_t i = 0; i < (num_threads - 1); ++i)
	{
		result += futures[i].get();
	}

	result += last_result;

	return result;
}

int main()
{
	const unsigned long long seed = 482001Ui64;
	const double radius = 1.0;

	std::mt19937_64 mt(seed);
	std::uniform_real_distribution <> urd(0.0, radius);
	
	const std::size_t size = 80'000'000U;

	std::vector<std::pair<double, double>> points(size, { 0.0, 0.0 });

	{
		for (auto& point : points)
		{
			point.first = urd(mt);
			point.second = urd(mt);
		}
	}

	auto lambda = [](std::pair<double, double>& point) {return (point.first * point.first + point.second * point.second < 1.0); };
	
	{
		Timer<std::chrono::microseconds> sequential_time("sequential time");

		auto inside = std::count_if(std::begin(points), std::end(points), lambda);		

		double pi = (2.0 * radius) * (2.0 * radius) * inside / size;
		std::cout << "pi = " << pi << '\n';
	}

	{
		Timer<std::chrono::microseconds> concurrent_time("concurrent time");		

		auto inside = parallel_count_if(std::begin(points), std::end(points), lambda);

		double pi = (2.0 * radius) * (2.0 * radius) * inside / size;
		std::cout << "pi = " << pi << '\n';
	}

	return 0;
}