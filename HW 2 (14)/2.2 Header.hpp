#pragma once

#include <chrono>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <iomanip>

template < typename Units >
class Timer
{
public:
	using clock_t = std::chrono::steady_clock;
	using time_point_t = clock_t::time_point;
	using duration_t = clock_t::duration;

	Timer(const std::string& name = "Timer") : m_begin(clock_t::now()), m_name(name), m_time_value(duration_t::zero()), is_stopped(false) {}

	~Timer() noexcept
	{
		try
		{
			stop();
			std::cout << m_name << ": " << std::chrono::duration_cast <Units> (m_time_value).count() << " " << units() << "\n";
		}
		catch (...)
		{
			std::cout << "\nDTOR ERROR!\n";
			std::abort();
		}
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
		m_time_value = duration_t::zero();
		is_stopped = false;
	}

	auto current_time()
	{
		stop();
		auto result = std::chrono::duration_cast <Units>(m_time_value).count();
		contin();
		return result;
	}

private:
	std::string units() const
	{
		switch (Units::period::den)
		{
		case 1'000'000'000:
			return "nanoseconds";
		case 1'000'000:
			return "microseconds";
		case 1'000:
			return "milliseconds";
		case 1:
			switch (Units::period::num)
			{
			case 1:
				return "seconds";
			case 60:
				return "minutes";
			case 3600:
				return "hours";
			default:
				return "unknown units\n";
			}
		default:
			return "unknown units\n";
		}
	}

	bool is_stopped;
	time_point_t m_begin;
	duration_t m_time_value;
	const std::string m_name;
};

struct Timer_result
{
	Timer_result() = default;

	Timer_result(const std::string& timer, int time) :
		m_name(timer), m_time(time), m_relative_time(-1.0), m_place(-1) {}

	Timer_result(Timer_result &other) = default;
	Timer_result(Timer_result &&other) = default;

	Timer_result& operator=(Timer_result& other) = default;
	Timer_result& operator=(Timer_result&& other) = default;

	~Timer_result() = default;

	std::string m_name;
	int m_time;
	int m_place;
	double m_relative_time;
};

std::ostream& operator<<(std::ostream& stream, Timer_result& timer_result)
{
	std::cout << std::left << std::setw(15) << std::setfill(' ') << timer_result.m_name
		<< std::setw(20) << std::setfill(' ') << timer_result.m_time
		<< std::setw(15) << std::setfill(' ') << std::setprecision(2) << std::fixed << timer_result.m_relative_time
		<< std::setw(7) << std::setfill(' ') << timer_result.m_place << '\n';
	return stream;
}