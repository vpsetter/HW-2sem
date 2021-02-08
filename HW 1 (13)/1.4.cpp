#include <chrono>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

template < typename Units >
class Timer
{
public:
	using clock_t = std::chrono::steady_clock;
	using time_point_t = clock_t::time_point;

	Timer(std::string name = "Timer") : m_begin(clock_t::now()), m_last_stop(m_begin), m_name(name) {}

	~Timer() = default;

	void stop()
	{
		if (m_last_stop == m_begin)
		{
			m_last_stop = clock_t::now();
		}
	}

	void contin() // by new m_begin value
	{
		if (m_last_stop != m_begin)
		{
			m_begin = clock_t::now() - m_last_stop + m_begin;
			m_last_stop = m_begin;
		}
	}

	void restart()
	{
		m_begin = clock_t::now();
		m_last_stop = m_begin;
	}

	void end()
	{
		stop();
		std::cout << m_name << ": " << std::chrono::duration_cast <Units> (m_last_stop - m_begin).count() << " " << units() << "\n";
	}

	int current_time()
	{
		stop();
		int result = std::chrono::duration_cast <Units> (m_last_stop - m_begin).count();
		contin();
		return result;
	}

private:
	std::string units()
	{
		std::string unit = "seconds";
		if (std::is_same<Units, std::chrono::nanoseconds>::value)
		{
			unit = "nanoseconds";
		}
		else if (std::is_same<Units, std::chrono::microseconds>::value)
		{
			unit = "microseconds";
		}
		else if (std::is_same<Units, std::chrono::milliseconds>::value)
		{
			unit = "milliseconds";
		}
		else if (std::is_same<Units, std::chrono::minutes>::value)
		{
			unit = "minutes";
		}
		else if (std::is_same<Units, std::chrono::hours>::value)
		{
			unit = "hours";
		}
		return unit;
	}

	time_point_t m_begin;
	time_point_t m_last_stop;
	std::string m_name;
};

int main()
{
	Timer<std::chrono::milliseconds> full_time("full_time");
	const auto N = 1000000U;
	Timer<std::chrono::milliseconds> vector1_changing_time("vector1_changing_time");	
	std::vector <int> v1(N, 0);	
	for (auto i = 0U; i < N; ++i)
	{
		v1[i] = i;
	}
	vector1_changing_time.stop();

	Timer<std::chrono::milliseconds> vector2_changing_time("vector2_changing_time");
	std::vector <int> v2(N, 0);
	for (auto i = 0U; i < N; ++i)
	{
		v2[i] = i;
	}
	vector2_changing_time.stop();

	auto desc = [](double x, double y) { return x > y; };

	vector1_changing_time.contin();
	std::sort(v1.begin(), v1.end(), desc);
	vector1_changing_time.stop();

	vector2_changing_time.contin();
	std::sort(v2.begin(), v2.end(), desc);
	vector2_changing_time.stop();

	std::cout << "current time without vectors: " << full_time.current_time() - vector1_changing_time.current_time() - vector2_changing_time.current_time() << " milliseconds\n";
	vector1_changing_time.end();
	vector2_changing_time.end();
	full_time.end();

	return 0;
}

