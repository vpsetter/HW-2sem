#include <algorithm>
#include <array>
#include <chrono>
#include <ctime>
#include <cstdint>
#include <iostream>
#include <utility>
#include <thread>
#include <vector>
#include <random>

#include <boost/asio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Text.hpp>

using namespace sf;

enum class Direction
{
	down,
	left,
	right,
	up
};

struct Player
{
	std::pair <std::size_t, std::size_t> position;
	Direction direction;
	Color color;
};

class Tron
{
public:
	Tron(Sprite background) :
		endpoint(boost::asio::ip::address_v4::any(), port),
		acceptor(io_service, endpoint.protocol()),
		socket(io_service),
		m_window(VideoMode(m_width, m_height), m_game_name),
		m_background(std::move(background)),
		m_delay(100'000)
	{
		connection_failure = establish_connection();

		for (auto column : m_field)
		{
			for (auto element : column) 
			{
				element = 0U;
			}
		}

		std::default_random_engine dre;
		std::uniform_int_distribution uid(1, 4);

		server_player.direction = static_cast<Direction>(uid(dre));
	};

	~Tron() = default;

private:
	int establish_connection()
	{
		try
		{
			acceptor.bind(endpoint);
			acceptor.listen(size);
			acceptor.accept(socket);
		}
		catch (boost::system::system_error& e)
		{
			std::cerr << "Error occured! Message: " << e.what() << std::endl;
			return e.code().value();
		}

		return 0;
	}

private:
	void draw()
	{
		m_window.draw(m_background);
		RectangleShape rect(Vector2f(m_cell_size, m_cell_size));
		for (auto i = 0U; i < m_field_width; ++i)
		{
			for (auto j = 0U; j < m_field_height; ++j)
			{
				switch (m_field[i][j])
				{
				case 1U:
					rect.setPosition(static_cast <float> (i * m_cell_size), static_cast <float> (j * m_cell_size));
					rect.setFillColor(server_player.color);
					m_window.draw(rect);
					break;
				case 2U:
					rect.setPosition(static_cast <float> (i * m_cell_size), static_cast <float> (j * m_cell_size));
					rect.setFillColor(client_player.color);
					m_window.draw(rect);
					break;
				default:
					break;
				}
			}
		}
		m_window.display();
	}

	void read_key()
	{
		if ((Keyboard::isKeyPressed(Keyboard::Left)) && (server_player.direction != Direction::right))
		{
			server_player.direction = Direction::left;
		}
		if ((Keyboard::isKeyPressed(Keyboard::Right)) && (server_player.direction != Direction::left))
		{
			server_player.direction = Direction::right;
		}
		if ((Keyboard::isKeyPressed(Keyboard::Up)) && (server_player.direction != Direction::down))
		{
			server_player.direction = Direction::up;
		}
		if ((Keyboard::isKeyPressed(Keyboard::Down)) && (server_player.direction != Direction::up))
		{
			server_player.direction = Direction::down;
		}
	}

	void send_direction()
	{
		std::string dir = "d\n";
		switch (server_player.direction)
		{
		case Direction::left:
			dir = "l\n";
			break;
		case Direction::right:
			dir = "r\n";
			break;
		case Direction::up:
			dir = "u\n";
			break;
		default: //case Direction::down:
			break;
		}
		boost::asio::write(socket, boost::asio::buffer(dir));
	}

	void accept_direction()
	{
		boost::asio::streambuf buffer;

		boost::asio::read_until(socket, buffer, '\n');
		std::istream input_stream(&buffer);

		char c = '0';
		input_stream >> c;
		switch (c)
		{
		case 'l':
			client_player.direction = Direction::left;
			break;
		case 'r':
			client_player.direction = Direction::right;
			break;
		case 'u':
			client_player.direction = Direction::up;
			break;
		default: //case 'd'
			client_player.direction = Direction::down;
			break;
		}
	}

	void update_position(Player& player)
	{
		switch (player.direction)
		{
		case Direction::left:
			player.position.first == 0U ? player.position.first = m_field_width - 1U : --player.position.first;
			break;
		case Direction::right:
			player.position.first == m_field_width - 1U ? player.position.first = 0U : ++player.position.first;
			break;
		case Direction::up:
			player.position.second == 0U ? player.position.second = m_field_height - 1U : --player.position.second;
			break;
		case Direction::down:
			player.position.second == m_field_height - 1U ? player.position.second = 0U : ++player.position.second;
			break;
		default:
			break;
		}
	}

	bool check_position()
	{
		if (server_player.position == client_player.position)
		{
			return true;
		}

		if (m_field[server_player.position.first][server_player.position.second] == 2U)
		{
			return true;
		}

		if (m_field[client_player.position.first][client_player.position.second] == 1U)
		{
			return true;
		}

		return false;
	}

	void update_field()
	{
		m_field[server_player.position.first][server_player.position.second] = 1U;
		m_field[client_player.position.first][client_player.position.second] = 2U;
	}

	void do_step()
	{
		update_position(server_player);
		update_position(client_player);

		is_ended = check_position();

		update_field();
	}


public:
	int start()
	{
		if (connection_failure)
		{
			return connection_failure;
		}

		m_background.setPosition(0.0, 0.0);

		m_window.clear();
		draw();

		auto time_point = std::chrono::steady_clock::now();

		while (!is_ended && m_window.isOpen())
		{
			Event event;

			while (m_window.pollEvent(event))
			{
				if (event.type == Event::Closed)
				{
					m_window.close();
				}
			}

			read_key();

			if (std::chrono::steady_clock::now() - time_point > m_delay)
			{
				accept_direction();
				send_direction();				
				do_step();

				time_point = std::chrono::steady_clock::now();
			}
			m_window.clear();
			draw();
		}

		if (is_ended)
		{
			Texture end_image;
			end_image.loadFromFile("images/end.png");
			m_background = Sprite(end_image);
			m_background.setPosition(0.0, 0.0);
			while (true)
			{
				m_window.clear();
				m_window.draw(m_background);
				m_window.display();

				Event event{};

				while (m_window.pollEvent(event))
				{
					if (event.type == Event::Closed)
					{
						m_window.close();
					}
				}
			}
		}

		return 0;
	}

private:
	const int size = 30;
	const std::size_t port = 8000U;
	boost::asio::ip::tcp::endpoint endpoint;
	boost::asio::io_service io_service;
	boost::asio::ip::tcp::acceptor acceptor;
	boost::asio::ip::tcp::socket socket;
	int connection_failure = 0;

	inline static const std::size_t m_cell_size = 10U;
	inline static const std::size_t m_width = 600U;
	inline static const std::size_t m_height = 480U;
	inline static const std::size_t m_field_width = m_width / m_cell_size;
	inline static const std::size_t m_field_height = m_height / m_cell_size;

	const std::string m_game_name = "TRON";
	Sprite m_background;
	RenderWindow m_window;
	std::array<std::array<std::size_t, m_field_height>, m_field_width> m_field;
	Direction m_server_direction;
	Direction m_client_direction;

	bool is_ended = false;

	Player server_player = { {10, 10}, Direction::up, /*{204, 102, 255}*/ Color::Red };
	Player client_player = { { m_field_width - 10U, m_field_height - 10U}, Direction::down, /*{ 255, 102, 0 }*/ Color::Green };

	const std::chrono::microseconds m_delay;
};

int main()
{
	Texture background_texture;
	background_texture.loadFromFile("images/background.jpg");
	Sprite background(background_texture);

	auto retVal = 0;

	retVal = Tron(background).start();

	system("pause");

	return retVal;
}