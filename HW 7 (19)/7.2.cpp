#include <algorithm>
#include <future>
#include <iostream>
#include <numeric>
#include <thread>
#include <vector>
#include <iomanip>

#include "Timer.hpp"

class Threads_Guard
{
public:

	explicit Threads_Guard(std::vector < std::thread >& threads) : m_threads(threads)
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

	std::vector < std::thread > & m_threads;
};

template < typename Iterator, typename T >
struct accumulate_block
{
	T operator()(Iterator first, Iterator last)
	{
		return std::accumulate(first, last, T());
	}
};

template < typename Iterator, typename T >
T parallel_count(Iterator first, Iterator last, T init, std::size_t num_threads)
{
	const std::size_t length = std::distance(first, last);

	if (!length) return init;

	const std::size_t block_size = length / num_threads;

	std::vector < std::future < T > > futures(num_threads - 1);
	std::vector < std::thread >		  threads(num_threads - 1);

	Threads_Guard guard(threads);

	Iterator block_start = first;

	for (std::size_t i = 0; i < (num_threads - 1); ++i)
	{
		Iterator block_end = block_start;
		std::advance(block_end, block_size);

		std::packaged_task < T(Iterator, Iterator) > task{
			accumulate_block < Iterator, T >() };

		futures[i] = task.get_future();
		threads[i] = std::thread(std::move(task), block_start, block_end);

		block_start = block_end;
	}

	T last_result = accumulate_block < Iterator, T >()(block_start, last);

	T result = init;

	for (std::size_t i = 0; i < (num_threads - 1); ++i)
	{
		result += futures[i].get();
	}

	result += last_result;

	return result;
}

int main(int argc, char** argv)
{
	std::vector < int > v(100'000'000, 1);
	const std::size_t threads_num_right_end = 1001U;

	{
		Timer<std::chrono::microseconds> sequence_time("Sequence time");
		std::cout << "Sequence result: " << std::accumulate(v.begin(), v.end(), 0) << '\n';
	}

	Timer<std::chrono::microseconds> concurrent_time("Concurrent time");

	std::cout << std::left << std::setw(20) << std::setfill(' ') << "Concurrent result"
		<< std::setw(20) << std::setfill(' ') << "Number of threads" << "Concurrent time (mcs)" << "\n\n";

	for (auto threads_num = 1U; threads_num < threads_num_right_end; ++threads_num)
	{
		concurrent_time.restart();
		std::cout << std::left << std::setw(20) << std::setfill(' ') << parallel_count(v.begin(), v.end(), 0, threads_num)
			<< std::setw(20) << std::setfill(' ') << threads_num << concurrent_time.current_time() << '\n';
	}

	return 0;
}