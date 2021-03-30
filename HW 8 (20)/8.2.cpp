#include <string>
#include <iterator>
#include <thread>
#include <future>
#include <algorithm>
#include <iostream>
#include <iomanip>

template < typename T >
class Threadsafe_vector
{
public:

	Threadsafe_vector() = default;

	Threadsafe_vector(const Threadsafe_vector& other)
	{
		std::lock_guard < std::mutex > lock(other.m_mutex);
		m_data = other.m_data;
	}

	Threadsafe_vector& operator=(const Threadsafe_vector&) = delete;

	operator std::vector<T>() const
	{
		return m_data;
	}

public:

	void push_back(T value)
	{
		std::lock_guard < std::mutex > lock(m_mutex);
		m_data.push_back(value);
	}

private:
	std::vector < T > m_data;

private:
	mutable std::mutex m_mutex;
};

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
	std::vector < std::thread >& m_threads;
};

struct find_substr_block
{
	void operator()(const std::string & string_to_find, const std::string & string,
		std::size_t first, std::size_t last, Threadsafe_vector<std::size_t>& indices)
	{
		auto begin = std::begin(string);
		auto length = string_to_find.size();
		for (auto index = first; index < last; ++index)
		{
			if (string_to_find == std::string(std::next(begin, index), std::next(begin, length + index)))
			{
				indices.push_back(index);
			}
		}
	}
};

std::vector<std::size_t> parallel_find_substr(const std::string & string_to_find, const std::string & string)
{
	Threadsafe_vector<std::size_t> indices;
	auto substring_length = string_to_find.size();
	auto string_length = string.size();
	if (string_length < substring_length + 32)
	{
		find_substr_block()(string_to_find, string, 0U, string_length - substring_length + 1, indices);
		return indices;
	}
	const std::size_t hardware_threads = std::thread::hardware_concurrency();
	const std::size_t num_threads = (hardware_threads != 0U ? hardware_threads : 2U);
	const std::size_t block_size = (string_length - substring_length) / num_threads;

	std::vector < std::future < void > > futures(num_threads - 1U);
	std::vector < std::thread > threads(num_threads - 1U);

	Threads_Guard guard(threads);

	auto first = 0U;

	for (std::size_t i = 0U; i < (num_threads - 1U); ++i)
	{
		auto last = first + block_size;

		std::packaged_task < void(const std::string &, const std::string&, std::size_t,
			std::size_t, Threadsafe_vector<std::size_t>&) > task{ find_substr_block() };

		futures[i] = task.get_future();
		threads[i] = std::thread(std::move(task), std::ref(string_to_find), std::ref(string), first, last, std::ref(indices));

		first = last;
	}

	find_substr_block()(string_to_find, string, first, string_length - substring_length + 1, indices);

	for (std::size_t i = 0U; i < (num_threads - 1U); ++i)
	{
		futures[i].get();
	}

	return indices;
}

int main()
{
	const std::string string = "AGCTTTTGCTAAATGCGGCTTAGGTGTCAGTGGTTTCGAGTCGTGGGTTTAAAATGTGTCCCGTTGAAAGTGGGGCCTTGAAATTCGTAGT";
	std::cout << "String: " << string << '\n' << "Enter a substring\n";
	std::string substring = "";
	std::cin >> substring;

	auto substring_length = substring.size();

	auto indices = parallel_find_substr(substring, string);

	std::sort(std::begin(indices), std::end(indices));

	std::cout << "Indices of the first symbols:\n";

	for (auto index : indices)
	{
		std::cout << index << '\n';
	}

	return 0;
}