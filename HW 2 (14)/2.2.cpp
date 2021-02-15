#include <array>
#include <list>
#include <forward_list>
#include <deque>

#include "2.2 Header.hpp"

int main()
{
	const std::size_t N = 100'000U;
	std::vector <int> vector(N, 0);
	std::array <int, N> array;
	std::deque <int> deque(N, 0);
	std::list <int> list;
	std::forward_list <int> forward_list;

	for (auto i = 0; i < N; ++i)
	{
		vector[i] = i;
		array[i] = i;
		deque[i] = i;
		list.push_back(i);
		forward_list.insert_after(forward_list.before_begin(), N - 1 - i);
	}
	
	using time_unit_t = std::chrono::microseconds;
	using Timer_t = Timer<time_unit_t>;
	auto desc = [](int x, int y) { return x > y; };
	auto asc_time = [](Timer_result& t1, Timer_result& t2) { return t1.m_time < t2.m_time; };

	std::vector<Timer_result> results;

	std::cout << "number of elements: " << N << "\n\n";

	{
		Timer_t vector_time("vector sort time");
		std::sort(vector.begin(), vector.end(), desc);
		results.push_back(Timer_result ("vector", vector_time.current_time()));
	}

	{
		Timer_t array_time("array sort time");
		std::sort(array.begin(), array.end(), desc);
		results.push_back(Timer_result("array", array_time.current_time()));
	}

	{
		Timer_t deque_time("deque sort time");
		std::sort(deque.begin(), deque.end(), desc);
		results.push_back(Timer_result("deque", deque_time.current_time()));
	}

	{
		Timer_t list_time("list sort time");
		list.sort();
		results.push_back(Timer_result("list", list_time.current_time()));
	}

	{
		Timer_t forward_list_time("forward_list sort time");
		forward_list.sort();
		results.push_back(Timer_result("forward_list", forward_list_time.current_time()));
	}

	std::sort(results.begin(), results.end(), asc_time);
	results.shrink_to_fit();
	for (auto i = 0U; i < results.capacity(); ++i)
	{
		results[i].m_place = i + 1;
		results[i].m_relative_time = static_cast <double> (results[i].m_time) / static_cast <double> (results[0].m_time);
	}

	std::cout << "\nranked list:\n\n";

	std::cout << std::left << std::setw(15) << std::setfill(' ') << "Container"
		<< std::setw(20) << std::setfill(' ') << "Time (microseconds)"
		<< std::setw(15) << std::setfill(' ') << "Relative time"
		<< std::setw(7) << std::setfill(' ') << "Place" << "\n\n";

	for (auto& result : results)
	{
		std::cout << result;
	}

	return 0;
}