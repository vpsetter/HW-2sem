#include <algorithm>
#include <array>
#include <thread>
#include <vector>
#include <random>
#include <atomic>
#include <execution>

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Text.hpp>

using namespace sf;

template <typename T>
class atomic_wrapper
{
public:
	atomic_wrapper() : m_atomic(){}

	atomic_wrapper(const std::atomic<T>& atomic) : m_atomic(atomic.load()){}

	atomic_wrapper(const T& var) : m_atomic(var) {}

	atomic_wrapper(const atomic_wrapper& other) : m_atomic(other.m_atomic.load()){}

	atomic_wrapper& operator=(const atomic_wrapper& other)
	{
		m_atomic.store(other.m_atomic.load());
		return *this;
	}


	operator T() const
	{
		return m_atomic;
	};

	atomic_wrapper& operator++()
	{
		++m_atomic;
		return *this;
	}

	atomic_wrapper& operator--()
	{
		--m_atomic;
		return *this;
	}

private:
	std::atomic<T> m_atomic;
};

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
		Particle(std::size_t x, std::size_t y, Direction dir) : position(x, y), direction(dir) {}

		std::pair <std::size_t, std::size_t> position;
		Direction direction;
	};

public:
	Visualizer_Brownian_Motion() :
		m_window(VideoMode(m_width, m_height), m_window_name),
		m_uid(0,3), m_bd(0.1),
		m_field(m_width, std::vector<atomic_wrapper<std::size_t>>(m_height, 0U))
	{
		std::uniform_int_distribution uid(0U, m_width - 1);
		for (auto i = 0U; i < m_particles_num; ++i)
		{
			auto x = uid(m_mt);
			auto y = uid(m_mt);
			auto dir = static_cast<Direction>(m_uid(m_mt));
			++m_field[x][y];
			m_particles.push_back(Particle(x, y, dir));
		}
	};

	~Visualizer_Brownian_Motion() noexcept = default;

private:
	std::size_t square_count(std::size_t i, std::size_t j)
	{
		std::atomic<std::size_t> result = 0U;
		auto begin_x = std::next(std::begin(m_field), i * m_squares_size);
		auto end_x = std::next(begin_x, m_squares_size);
		std::for_each(std::execution::par, begin_x, end_x, [this, j, &result](auto& column)
			{
				auto begin_y = std::next(std::begin(column), j * m_squares_size);
				auto end_y = std::next(begin_y, m_squares_size);

				result += std::accumulate(begin_y, end_y, 0U);
			});
		return result;
	}

	void draw()
	{
		RectangleShape rect(Vector2f(m_squares_size, m_squares_size));
		for (auto i = 0U; i < m_squares_in_column; ++i)
		{
			for (auto j = 0U; j < m_squares_in_column; ++j)
			{
				Color color;
				const std::size_t particles_num = square_count(i,j);
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
				rect.setPosition(static_cast <float> (i * m_squares_size), static_cast <float> (j * m_squares_size));
				rect.setFillColor(color);
				m_window.draw(rect);
			}
		}
		m_window.display();
	}

	void step(Particle& particle)
	{
		switch (particle.direction)
		{
		case Direction::left:
			if (particle.position.first == 0U) 
			{
				particle.direction = Direction::right;
			}
			else 
			{
				--m_field[particle.position.first][particle.position.second];
				--particle.position.first;
				++m_field[particle.position.first][particle.position.second];
			}			
			break;
		case Direction::right:
			if (particle.position.first == m_width - 1U)
			{
				particle.direction = Direction::left;
			}
			else
			{
				--m_field[particle.position.first][particle.position.second];
				++particle.position.first;
				++m_field[particle.position.first][particle.position.second];
			}
			break;
		case Direction::up:
			if (particle.position.second == 0U)
			{
				particle.direction = Direction::down;
			}
			else
			{
				--m_field[particle.position.first][particle.position.second];
				--particle.position.second;
				++m_field[particle.position.first][particle.position.second];
			}
			break;
		case Direction::down:
			if (particle.position.second == m_height - 1U)
			{
				particle.direction = Direction::up;
			}
			else
			{
				--m_field[particle.position.first][particle.position.second];
				++particle.position.second;
				++m_field[particle.position.first][particle.position.second];
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
		std::for_each(std::execution::par, std::begin(m_particles), std::end(m_particles), [this](auto& particle) {step(particle); });
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
	std::vector<std::vector<atomic_wrapper<std::size_t>>> m_field;

	const std::string m_window_name = "Brownian motion";
	RenderWindow m_window;

	inline static const std::size_t m_colors_num = 5U;
	const std::array<Color, m_colors_num> m_colors = 
	{Color(50, 0, 0), Color(100, 0, 0), Color(150, 0, 0), Color(200, 0, 0), Color(250, 0, 0)};
	inline static const std::size_t m_change_color_particles_num = 5U;

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