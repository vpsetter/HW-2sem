#include <iostream>

#include <boost/asio.hpp>

void read_data(boost::asio::ip::tcp::socket& socket)
{
	boost::asio::streambuf buffer;
	std::string message = "";

	do
	{
		boost::asio::read_until(socket, buffer, '\n');
		std::istream input_stream(&buffer);

		std::getline(input_stream, message, '\n');
		std::cout << message << '\n';
	} while (message != "your interlocator left the chat");
}

void write_data(boost::asio::ip::tcp::socket& socket, const std::string& name)
{
	std::string data;
	std::getline(std::cin, data);

	while (data != "exit")
	{
		boost::asio::write(socket, boost::asio::buffer(name + data + '\n'));
		std::getline(std::cin, data);
	}
	boost::asio::write(socket, boost::asio::buffer("your interlocator left the chat\n"));
}

int main(int argc, char** argv)
{
	std::string raw_ip_address = "192.168.0.101";

	auto port = 8000;
	std::string name = "";
	std::cout << "What's your name?\n";
	std::getline(std::cin, name);
	name += ": ";

	try
	{
		boost::asio::ip::tcp::endpoint endpoint(
			boost::asio::ip::address::from_string(raw_ip_address), port);

		boost::asio::io_service io_service;

		boost::asio::ip::tcp::socket socket(io_service, endpoint.protocol());

		socket.connect(endpoint);

		auto reader = std::thread(read_data, std::ref(socket));
		write_data(socket, name);
		reader.join();
	}
	catch (boost::system::system_error& e)
	{
		std::cout << "Error occured! Error code = " << e.code() << ". Message: " << e.what() << std::endl;

		system("pause");

		return e.code().value();
	}

	system("pause");

	return 0;
}