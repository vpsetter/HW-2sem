#include <algorithm>
#include <array>
#include <thread>
#include <vector>
#include <random>

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Text.hpp>

using namespace sf;

class Visualizer_Brownian_Motion
{
private:
	enum class Direction
	{
		down,
		left,
		right,
		up
	};

	struct Particle
	{
		Particle() = default;
		Particle(int coord_1, int coord_2, Direction dir) : x(coord_1), y(coord_2), direction(dir) {}

		int x;
		int y;
		Direction direction;
	};

public:
	Visualizer_Brownian_Motion() :
		m_window(VideoMode(m_width, m_height), m_window_name),
		m_uid(0,3), m_bd(0.1),
		m_field(m_squares_in_column, std::vector<std::size_t>(m_squares_in_column, 0U))
	{
		std::uniform_int_distribution uid(0, static_cast<int>(m_width) - 1);
		m_particles.reserve(m_particles_num);
		for (auto i = 0U; i < m_particles_num; ++i)
		{
			auto x = uid(m_mt);
			auto y = uid(m_mt);
			auto dir = static_cast<Direction>(m_uid(m_mt));
			++m_field[x/m_squares_size][y/m_squares_size];
			m_particles.emplace(std::end(m_particles), Particle(x, y, dir));
		}
	};

	~Visualizer_Brownian_Motion() noexcept = default;

private:

	void draw()
	{
		for(auto& column : m_field)
		{
			for (auto& cell : column)
			{
				cell = 0U;
			}
		}
		std::for_each(std::begin(m_particles), std::end(m_particles), [this](const Particle& particle)
			{
				++m_field[particle.x / m_squares_size][particle.y / m_squares_size];
			});
		RectangleShape big_rect(Vector2f(m_squares_size, m_squares_size));
		for (auto i = 0U; i < m_squares_in_column; ++i)
		{
			for (auto j = 0U; j < m_squares_in_column; ++j)
			{
				Color color;
				const std::size_t particles_num = m_field[i][j];
				if (!particles_num)
				{
					color = Color::Black;
				}
				else
				{
					auto saturation = particles_num / m_change_color_particles_num;
					saturation = (saturation > m_colors_num - 1 ? m_colors_num - 1 : saturation);
					color = m_colors[saturation];
				}
				big_rect.setPosition(static_cast <float> (i * m_squares_size), static_cast <float> (j * m_squares_size));
				big_rect.setFillColor(color);
				m_window.draw(big_rect);
			}
		}
		RectangleShape small_rect(Vector2f(1.0f, 1.0f));
		std::for_each(std::begin(m_particles), std::end(m_particles), [this, &small_rect](const Particle& particle)
			{
				small_rect.setPosition(static_cast<float> (particle.x), static_cast<float>(particle.y));
				small_rect.setFillColor(m_particle_color);
				m_window.draw(small_rect);
			});
		m_window.display();
	}

	void step(Particle& particle)
	{
		switch (particle.direction)
		{
		case Direction::left:
			if (particle.x == 0U) 
			{
				particle.direction = Direction::right;
			}
			else 
			{
				--particle.x;
			}			
			break;
		case Direction::right:
			if (particle.x == m_width - 1)
			{
				particle.direction = Direction::left;
			}
			else
			{
				++particle.x;
			}
			break;
		case Direction::up:
			if (particle.y == 0U)
			{
				particle.direction = Direction::down;
			}
			else
			{
				--particle.y;
			}
			break;
		case Direction::down:
			if (particle.y == m_height - 1)
			{
				particle.direction = Direction::up;
			}
			else
			{
				++particle.y;
			}
			break;
		default:
			break;
		}
		if (m_bd(m_mt))
		{
			particle.direction = static_cast<Direction>(m_uid(m_mt));
		}
	}

	void move()
	{
		std::for_each(std::begin(m_particles), std::end(m_particles), [this](auto& particle) {step(particle); });
	}

public:
	void start_motion()
	{
		while (m_window.isOpen())
		{
			Event event;

			while (m_window.pollEvent(event))
			{
				if (event.type == Event::Closed)
				{
					m_window.close();
				}
			}
			move();
			m_window.clear();
			draw();
		}
	}

private:

	std::mt19937_64 m_mt;
	std::uniform_int_distribution<> m_uid;
	std::bernoulli_distribution m_bd;

	inline static const std::size_t m_width = 1000U;
	inline static const std::size_t m_height = 1000U;
	inline static const std::size_t m_particles_num = 1000U;

	std::vector<Particle> m_particles;
	std::vector<std::vector<std::size_t>> m_field;

	const std::string m_window_name = "Brownian motion";
	RenderWindow m_window;

	inline static const std::size_t m_colors_num = 5U;
	const std::array<Color, m_colors_num> m_colors = 
	{Color(50, 0, 0), Color(100, 0, 0), Color(150, 0, 0), Color(200, 0, 0), Color(250, 0, 0)};
	inline static const std::size_t m_change_color_particles_num = 5U;
	const Color m_particle_color = { 0, 255, 255 };

	inline static const std::size_t m_squares_in_column = 10U;
	inline static const std::size_t m_squares_size = m_height / m_squares_in_column;
};

int main()
{
	Visualizer_Brownian_Motion visualizer;

	visualizer.start_motion();

	system("pause");

	return 0;
}