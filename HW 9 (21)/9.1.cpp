#define BOOST_DATE_TIME_NO_LIB

#include <iostream>
#include <string>
#include <mutex>
#include <list>
#include <future>
#include <thread>

#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/containers/string.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/interprocess_condition.hpp>

using segment_manager_t = boost::interprocess::managed_shared_memory::segment_manager;
using string_allocator_t = boost::interprocess::allocator <char, segment_manager_t>;
using string_t = boost::interprocess::basic_string < char, std::char_traits < char >, string_allocator_t>;
using map_value_t = std::pair <const std::size_t, string_t>;
using map_allocator_t = boost::interprocess::allocator<map_value_t, segment_manager_t>;
using map_t = boost::interprocess::map<std::size_t, string_t, std::less<std::size_t>, map_allocator_t>;


void read(boost::interprocess::interprocess_mutex* mutex_ptr, boost::interprocess::interprocess_condition* condition_ptr,
	map_t* messages, std::size_t& current_message_to_read,	std::atomic<bool>& done)
{
	std::unique_lock lock(*mutex_ptr);
	while (!done)
	{
		condition_ptr->wait(lock, [&done, &current_message_to_read, messages]() 
			{return done || current_message_to_read < messages->size(); });

		std::cout << messages->find(current_message_to_read++)->second << '\n';
	}
}

void write(std::atomic<std::size_t>* process_id, boost::interprocess::interprocess_mutex* mutex_ptr, boost::interprocess::interprocess_condition* condition_ptr,
	map_t* messages, std::size_t& current_message_to_read, std::atomic<bool>& done, boost::interprocess::managed_shared_memory& mshm)
{
	std::string str = "";
	std::string info = "process " + std::to_string(*process_id);
	std::string info_said = info + " said: ";
	{
		const std::string output = info + " joined the chat";
		std::cout << output << '\n';
		std::scoped_lock lock(*mutex_ptr);
		string_t string(output.c_str(), mshm.get_segment_manager());
		map_value_t message(messages->size(), string);
		messages->insert(message);
		++current_message_to_read;
		condition_ptr->notify_all();
	}
	std::getline(std::cin, str);
	while (str != "exit" && messages->size() < 500U)
	{
		{
			std::scoped_lock lock(*mutex_ptr);
			string_t string((info_said + str).c_str(), mshm.get_segment_manager());
			map_value_t message(messages->size(), string);
			messages->insert(message);
			++current_message_to_read;
			condition_ptr->notify_all();
		}
		std::getline(std::cin, str);
	}
	{
		std::scoped_lock lock(*mutex_ptr);
		string_t string((info + " left the chat").c_str(), mshm.get_segment_manager());
		map_value_t message(messages->size(), string);
		messages->insert(message);
		++current_message_to_read;
		done.store(true);
		condition_ptr->notify_all();
	}
}

int main()
{
	system("pause");
	const std::string managed_shared_memory_name = "managed_shared_memory";

	boost::interprocess::managed_shared_memory managed_shared_memory(
		boost::interprocess::open_or_create, managed_shared_memory_name.c_str(), 65536);

	auto counter_ptr = managed_shared_memory.find_or_construct <std::atomic<std::size_t>>("counter")();
	auto process_id = managed_shared_memory.find_or_construct <std::atomic<std::size_t>>("process_id")();
	++(*counter_ptr);
	++(*process_id);

	auto messages = managed_shared_memory.find_or_construct <map_t>("messages")(std::less<std::size_t>(), managed_shared_memory.get_segment_manager());

	auto mutex_ptr = managed_shared_memory.find_or_construct <boost::interprocess::interprocess_mutex >("mutex_ptr")();
	auto condition_variable_ptr = managed_shared_memory.find_or_construct <boost::interprocess::interprocess_condition >("condition_variable_ptr")();

	std::size_t current_message_to_read = 0U;

	std::atomic<bool> done(false);

	for (; current_message_to_read < messages->size(); std::cout << messages->find(current_message_to_read++)->second << '\n');

	std::thread thread_read(read, mutex_ptr, condition_variable_ptr, messages,
		std::ref(current_message_to_read), std::ref(done));
	std::thread thread_write(write, process_id, mutex_ptr, condition_variable_ptr, messages,
		std::ref(current_message_to_read), std::ref(done), std::ref(managed_shared_memory));

	thread_write.join();
	thread_read.join();

	--(*counter_ptr);
	if (*counter_ptr == 0)
	{
		boost::interprocess::shared_memory_object::remove(managed_shared_memory_name.c_str());
	}

	return 0;
}

