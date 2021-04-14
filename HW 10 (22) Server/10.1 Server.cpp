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
	boost::asio::write(socket, boost::asio::buffer("Server is ready to answer your questions\n"));
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
	const std::size_t size = 30;

	auto port = 8000;

	boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address_v4::any(), port);

	boost::asio::io_service io_service;

	std::string name = "Server: ";

	try
	{
		boost::asio::ip::tcp::acceptor acceptor(io_service, endpoint.protocol());

		acceptor.bind(endpoint);

		acceptor.listen(size);

		boost::asio::ip::tcp::socket socket(io_service);

		acceptor.accept(socket);

		auto reader = std::thread(read_data, std::ref(socket));
		write_data(socket, std::cref(name));
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