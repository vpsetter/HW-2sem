#include <chrono>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

//#include <boost/type_index.hpp>

template < typename Units >
class Timer
{
public:
	using clock_t = std::chrono::steady_clock;
	using time_point_t = clock_t::time_point;
	using duration_t = clock_t::duration;

	Timer(const std::string& name = "Timer") : m_begin(clock_t::now()), m_name(name), m_time_value(0), is_stopped (false) {}

	~Timer()
	{
		stop();
		std::cout << m_name << ": " << std::chrono::duration_cast <Units> (m_time_value).count() << " " << units() << "\n";
	};

	void stop()
	{
		if (!is_stopped)
		{
			m_time_value += (clock_t::now() - m_begin);
			is_stopped = true;
		}
	}

	void contin() // by new m_begin value
	{
		if (is_stopped)
		{
			m_begin = clock_t::now();
			is_stopped = false;
		}
	}

	void restart()
	{
		m_begin = clock_t::now();
		m_time_value = 0;
		is_stopped = false;
	}

	int current_time()
	{
		stop();
		int result = std::chrono::duration_cast <Units>(m_time_value).count();
		contin();
		return result;
	}

private:
	std::string units()
	{
		std::string unit = "";
		switch (Units::period::den)
		{
		case 1'000'000'000:
			unit = "nanoseconds";
			break;
		case 1'000'000:
			unit = "microseconds";
			break;
		case 1'000:
			unit = "milliseconds";
			break;
		default:
			switch (Units::period::num)
			{
			case 60:
				unit = "minutes";
				break;
			case 3600:
				unit = "hours";
				break;
			default:
				unit = "seconds";
				break;
			}
		}

		return unit;
	}

	bool is_stopped;
	time_point_t m_begin;
	duration_t m_time_value;
	const std::string m_name;
};

int main()
{
	{
		Timer<std::chrono::milliseconds> full_time("full_time");
		const auto N = 1000000U;
		{
			Timer<std::chrono::milliseconds> vector1_changing_time("vector1_changing_time");
			std::vector <int> v1(N, 0);
			for (auto i = 0U; i < N; ++i)
			{
				v1[i] = i;
			}
			vector1_changing_time.stop();

			{
				Timer<std::chrono::milliseconds> vector2_changing_time("vector2_changing_time");
				std::vector <int> v2(N, 0);
				for (auto i = 0U; i < N; ++i)
				{
					v2[i] = i;
				}
				vector2_changing_time.stop();

				auto desc = [](int x, int y) { return x > y; };

				vector1_changing_time.contin();
				std::sort(v1.begin(), v1.end(), desc);
				vector1_changing_time.stop();

				vector2_changing_time.contin();
				std::sort(v2.begin(), v2.end(), desc);
				vector2_changing_time.stop();
				std::cout << "current time without vectors: " << full_time.current_time() - vector1_changing_time.current_time() - vector2_changing_time.current_time() << " milliseconds\n";
			}
		}
		
		/*std::cout << boost::typeindex::type_id<std::chrono::milliseconds>().pretty_name(); // less beautifully */
	}

	return 0;
}

