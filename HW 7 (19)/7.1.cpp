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
			std::cerr << "\nDTOR THREAD ERROR\n";
		}
	}

private:
	std::vector < std::thread >& m_threads;
};

template < typename F, typename Distr_t>
struct count_points
{
	std::size_t operator()(const std::size_t N, F predicate, Distr_t distribution)
	{
		std::mt19937_64 generator(std::hash<std::thread::id>{}(std::this_thread::get_id()));
		std::pair <double, double> point = { 0.0, 0.0 };
		std::size_t result = 0U;
		for (auto i = 0U; i < N; ++i)
		{
			point.first = distribution(generator);
			point.second = distribution(generator);
			if (predicate(point))
			{
				++result;
			}
		}
		return result;
	}
};

template <typename F, typename Distr_t >
std::size_t parallel_count(const std::size_t points_num, F predicate, Distr_t distribution)
{
	const std::size_t hardware_threads = std::thread::hardware_concurrency();
	const std::size_t num_threads =	(hardware_threads != 0U ? hardware_threads : 2U);
	const std::size_t block_size = points_num / num_threads;

	std::vector < std::future < std::size_t > > futures(num_threads - 1U);
	std::vector < std::thread > threads(num_threads - 1U);

	Threads_Guard guard(threads);

	for (std::size_t i = 0U; i < (num_threads - 1U); ++i)
	{
		std::packaged_task < std::size_t(std::size_t, F, Distr_t) > task{count_points <F, Distr_t>()};

		futures[i] = task.get_future();
		threads[i] = std::thread(std::move(task), block_size, predicate, distribution);
	}

	std::size_t last_result = count_points < F, Distr_t >()(points_num - (num_threads - 1U) * block_size, predicate, distribution);

	std::size_t result = 0U;

	for (std::size_t i = 0U; i < (num_threads - 1U); ++i)
	{
		result += futures[i].get();
	}

	result += last_result;

	return result;
}

int main()
{
	const double radius = 1.0;	
	const std::size_t points_num = 80'000'000U;

	std::uniform_real_distribution <> urd(0.0, radius);

	auto lambda = [](std::pair<double, double>& point) {return (point.first * point.first + point.second * point.second < 1.0); };
	
	{
		Timer<std::chrono::milliseconds> sequential_time("sequential time");

		auto inside = count_points <decltype(lambda), decltype(urd)>()(points_num, lambda, urd);		

		double pi = 4.0 * radius * radius * inside / points_num;
		std::cout << "pi = " << pi << '\n';
	}

	{
		Timer<std::chrono::milliseconds> concurrent_time("concurrent time");

		std::vector<std::pair<double, double>> points(points_num, { 0.0, 0.0 });

		auto inside = parallel_count(points_num, lambda, urd);

		double pi = 4.0 * radius * radius * inside / points_num;
		std::cout << "pi = " << pi << '\n';
	}

	return 0;
}