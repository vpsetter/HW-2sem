#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>
#include <iostream>

template < typename T, typename Container = std::vector<T>,	typename Compare = std::less<typename Container::value_type>>
class Threadsafe_Priority_Queue
{
public:

	Threadsafe_Priority_Queue() = default;

	Threadsafe_Priority_Queue(const Threadsafe_Priority_Queue& other)
	{
		std::lock_guard < std::mutex > lock(other.m_mutex);
		m_priority_queue = other.m_priority_queue;
	}

	Threadsafe_Priority_Queue& operator=(const Threadsafe_Priority_Queue& other)
	{
		std::scoped_lock lock(m_mutex, other.m_mutex);
		m_priority_queue = other.m_priority_queue;
	}

public:

	void push(T value)
	{
		std::lock_guard < std::mutex > lock(m_mutex);
		m_priority_queue.push(value);
		m_condition_variable.notify_one();
	}

	void wait_and_pop(T& value)
	{
		std::unique_lock < std::mutex > lock(m_mutex);

		m_condition_variable.wait(lock, [this] {return !m_priority_queue.empty(); });
		value = m_priority_queue.top();
		m_priority_queue.pop();
	}

	std::shared_ptr < T > wait_and_pop()
	{
		std::unique_lock < std::mutex > lock(m_mutex);

		m_condition_variable.wait(lock, [this] {return !m_priority_queue.empty(); });
		auto result = std::make_shared < T >(m_priority_queue.top());
		m_priority_queue.pop();

		return result;
	}

	bool try_pop(T& value)
	{
		std::lock_guard < std::mutex > lock(m_mutex);

		if (m_priority_queue.empty())
		{
			return false;
		}

		value = m_priority_queue.top();
		m_priority_queue.pop();

		return true;
	}

	std::shared_ptr < T > try_pop()
	{
		std::lock_guard < std::mutex > lock(m_mutex);

		if (m_priority_queue.empty())
		{
			return std::shared_ptr < T >();
		}

		auto result = std::make_shared < T >(m_priority_queue.top());
		m_priority_queue.pop();

		return result;
	}

	bool empty() const
	{
		std::lock_guard < std::mutex > lock(m_mutex);
		return m_priority_queue.empty();
	}

private:

	std::priority_queue < T, Container , Compare > m_priority_queue;
	std::condition_variable m_condition_variable;

private:

	mutable std::mutex m_mutex;
};

int main(int argc, char** argv)
{
	Threadsafe_Priority_Queue < int > priority_queue;

	int variable1 = 42, variable2 = 12, variable3 = 165;
	priority_queue.push(variable1);
	priority_queue.push(variable2);
	priority_queue.push(variable3);

	std::cout << "pushed: " << variable1 << ' ' << variable2 << ' ' << variable3 << '\n';

	auto first_ptr = priority_queue.wait_and_pop();

	std::cout << "pop1: ";
	if (first_ptr)
	{
		std::cout << "success: " << *first_ptr << '\n';
	}
	else
	{
		std::cout << "failure\n";
	}

	int second = 0;
	bool result = priority_queue.try_pop(second);

	std::cout << "pop2: ";
	if (result)
	{
		std::cout << "success: " << second << '\n';
	}
	else
	{
		std::cout << "failure\n";
	}

	int third = 0;
	priority_queue.wait_and_pop(third);
	std::cout << "pop3: " << third;

	return 0;
}
