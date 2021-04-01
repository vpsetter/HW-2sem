#include <exception>
#include <memory>
#include <mutex>
#include <stack>
#include <stdexcept>
#include <condition_variable>
#include <queue>
#include <iostream>
#include <future>
#include <thread>

#include <boost/lockfree/queue.hpp>
#include <boost/lockfree/stack.hpp>

#include "Timer.hpp"

template < typename T >
class Threadsafe_Stack
{
public:

	Threadsafe_Stack() = default;

	Threadsafe_Stack(const Threadsafe_Stack& other)
	{
		std::lock_guard < std::mutex > lock(other.m_mutex);
		m_data = other.m_data;
	}

	Threadsafe_Stack& operator=(const Threadsafe_Stack&) = delete;

public:

	bool push(T value)
	{
		std::lock_guard < std::mutex > lock(m_mutex);
		m_data.push(value);
		return true;
	}

	bool pop(T& value)
	{
		std::lock_guard < std::mutex > lock(m_mutex);

		if (m_data.empty())
		{
			return false;
		}

		value = m_data.top();
		m_data.pop();

		return true;
	}

	bool empty() const
	{
		std::lock_guard < std::mutex > lock(m_mutex);
		return m_data.empty();
	}

private:

	std::stack < T > m_data;

private:

	mutable std::mutex m_mutex;
};

template < typename T >
class Threadsafe_Queue
{
public:

	Threadsafe_Queue() = default;

	Threadsafe_Queue(const Threadsafe_Queue& other)
	{
		std::lock_guard < std::mutex > lock(other.m_mutex);
		m_queue = other.m_queue;
	}

	Threadsafe_Queue& operator=(const Threadsafe_Queue& other)
	{
		std::scoped_lock lock(m_mutex, other.m_mutex);
		m_queue = other.m_queue;
	}

public:

	bool push(T value)
	{
		std::lock_guard < std::mutex > lock(m_mutex);
		m_queue.push(value);
		return true;
	}

	bool pop(T& value)
	{
		std::lock_guard < std::mutex > lock(m_mutex);

		if (m_queue.empty())
		{
			return false;
		}

		value = m_queue.front();
		m_queue.pop();

		return true;
	}

	bool empty() const
	{
		std::lock_guard < std::mutex > lock(m_mutex);
		return m_queue.empty();
	}

private:

	std::queue < T > m_queue;

private:

	mutable std::mutex m_mutex;
};

template<typename Container>
void producer(std::size_t M, Container &container, std::atomic<std::size_t> &push_counter, std::atomic<bool>& flag)
{
	while (!flag.load())
	{
		std::this_thread::yield();
	}

	for (int i = 0; i != M; ++i) 
	{
		while (!container.push(i)) {}
		++push_counter;
	}
}

template<typename Container>
void consumer(std::size_t M, Container& container, std::atomic<std::size_t> &pop_counter, std::atomic<bool>& flag)
{
	while (!flag.load())
	{
		std::this_thread::yield();
	}

	int value = 0;
	for(auto consumed = 0U; consumed != M; ++consumed)
	{
		while (!container.pop(value));
		++pop_counter;
	}
}

template<typename Container>
void parallel_operation(std::size_t N, std::size_t M, Container &container,
	std::atomic<std::size_t>& push_counter, std::atomic<std::size_t>& pop_counter, std::atomic<bool>& flag)
{
	std::vector < std::thread > threads;

	for (std::size_t i = 0U; i < N; ++i)
	{
		threads.push_back(std::thread(producer<Container>, M, std::ref(container), std::ref(push_counter), std::ref(flag)));
		threads.push_back(std::thread(consumer<Container>, M, std::ref(container), std::ref(pop_counter), std::ref(flag)));
	}

	flag.store(true);

	std::for_each(threads.begin(), threads.end(), std::mem_fn(&std::thread::join));
}

void default_conditions(std::atomic<bool>& flag, std::atomic<std::size_t>& push_counter, std::atomic<std::size_t>& pop_counter)
{
	flag.store(false);
	push_counter = 0U;
	pop_counter = 0U;
}

int main()
{
	std::atomic<bool> flag(false);
	std::atomic<std::size_t> push_counter = 0U;
	std::atomic<std::size_t> pop_counter = 0U;

	std::vector<std::size_t> Ms = { 100U, 1'000U, 10'000U, 100'000U, 1'000'000U};

	for (auto N = 1U; N < 9U; ++N)
	{ 
		for (auto M : Ms)
		{
			std::cout << "N = " << N << " M = " << M << '\n';
			{
				Timer<std::chrono::microseconds>timer("queue_lock");
				Threadsafe_Queue<int> queue;
				parallel_operation(N, M, queue, push_counter, pop_counter, flag);
				std::cout << ((pop_counter == push_counter) ? "success " : "failure ");
				default_conditions(flag, push_counter, pop_counter);
			}
			{
				Timer<std::chrono::microseconds>timer("stack_lock");
				Threadsafe_Stack<int> stack;
				parallel_operation(N, M, stack, push_counter, pop_counter, flag);
				std::cout << ((pop_counter == push_counter) ? "success " : "failure ");
				default_conditions(flag, push_counter, pop_counter);
			}
			{
				Timer<std::chrono::microseconds>timer("queue_lockfree");
				boost::lockfree::queue<int> queue(128);
				parallel_operation(N, M, queue, push_counter, pop_counter, flag);
				std::cout << ((pop_counter == push_counter) ? "success " : "failure ");
				default_conditions(flag, push_counter, pop_counter);
			}
			{
				Timer<std::chrono::microseconds>timer("stack_lockfree");
				boost::lockfree::stack<int> stack(128);
				parallel_operation(N, M, stack, push_counter, pop_counter, flag);
				std::cout << ((pop_counter == push_counter) ? "success " : "failure ");
				default_conditions(flag, push_counter, pop_counter);
			}
		}
	}


	return 0;
}