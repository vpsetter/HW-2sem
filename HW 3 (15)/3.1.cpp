#include <iostream>
#include <vector>
#include <set>
#include <array>

#include "3.1 Header.hpp"

template <const std::size_t N, typename Units>
std::array <int, 3U> time_count(Timer<Units> &vec_timer, Timer<Units> &arr_timer, Timer<Units> &set_timer)
{
	std::array <int, 3U> results;

	set_timer.restart();
	std::set<int> set;
	for (int i = N; i > 0; --i)
	{
		set.insert(i);
	}
	set_timer.stop();
	results[0] = set_timer.current_time();

	vec_timer.restart();
	std::vector<int> vector(N, 0);
	for (int i = 0; i < N; ++i)
	{
		vector[i] = N - i;
	}
	std::sort(vector.begin(), vector.end());
	vec_timer.stop();
	results[1] = vec_timer.current_time();

	arr_timer.restart();
	std::array<int, N> array;
	for (int i = 0; i < N; ++i)
	{
		array[i] = N - i;
	}
	std::sort(array.begin(), array.end());
	arr_timer.stop();		
	results[2] = arr_timer.current_time();

	return results;
}

template <std::size_t N, typename Units>
void print_times(Timer<Units>& vec_timer, Timer<Units>& arr_timer, Timer<Units>& set_timer, std::ostream &stream)
{
	if constexpr (N < 250'000U)
	{
		std::array <int, 3U> results = time_count<N>(vec_timer, arr_timer, set_timer);
		stream << std::left << std::setw(15) << std::setfill(' ') << N
			<< std::setw(20) << std::setfill(' ') << results[0]
			<< std::setw(20) << std::setfill(' ') << results[1]
			<< std::setw(20) << std::setfill(' ') << results[2] << '\n';
		print_times<2 * N>(vec_timer, arr_timer, set_timer, stream);
	}	
}

int main()
{
	Timer<std::chrono::microseconds> vector_timer("vector_timer");
	Timer<std::chrono::microseconds> array_timer("array_timer");
	Timer<std::chrono::microseconds> set_timer("set_timer");

	std::cout << std::left << std::setw(15) << std::setfill(' ') << "N (elements)"
		<< std::setw(20) << std::setfill(' ') << "set time (mcs)"
		<< std::setw(20) << std::setfill(' ') << "vector time (mcs)"
		<< std::setw(20) << std::setfill(' ') << "array time (mcs)" << '\n';

	print_times<5U, std::chrono::microseconds>(vector_timer, array_timer, set_timer, std::cout);

	// 1.array, 2.vector 3.set always

	return 0;
}