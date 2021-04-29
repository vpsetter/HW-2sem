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
		m_uid(1,4), m_bd(0.1),
		m_field(m_width, std::vector<atomic_wrapper<std::size_t>>(m_height, 0U))
	{
		m_image.create(m_width, m_height);
		std::uniform_int_distribution uid(0U, m_width - 1);
		for (auto i = 0U; i < m_particles_num; ++i)
		{
			auto x = uid(m_mt);
			auto y = uid(m_mt);
			auto dir = static_cast<Direction>(m_uid(m_mt));
			++m_field[x][y];
			m_particles.push_back(Particle( x, y, dir));
		}
	};

	~Visualizer_Brownian_Motion() noexcept = default;

private:
	void draw()
	{
		for (auto i = 0U; i < m_width; ++i)
		{
			for (auto j = 0U; j < m_height; ++j)
			{
				const std::size_t particles_num = m_field[i][j];
				if (!particles_num)
				{
					m_image.setPixel(i, j, Color::Black);
				}
				else
				{
					auto saturation = particles_num / change_color_particles_num;
					saturation = (saturation > colors_num - 1 ? colors_num - 1 : saturation);
					m_image.setPixel(i, j, m_colors[saturation]);
				}
			}
		}
		m_texture.loadFromImage(m_image);
		m_sprite.setTexture(m_texture, true);
		m_window.draw(m_sprite);
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
	inline static const std::size_t m_particles_num = 10'000'000U;

	std::vector<Particle> m_particles;
	std::vector<std::vector<atomic_wrapper<std::size_t>>> m_field;

	const std::string m_window_name = "Brownian motion";
	RenderWindow m_window;
	Image m_image;
	Sprite m_sprite;
	Texture m_texture;

	inline static const std::size_t colors_num = 5U;
	const std::array<Color, colors_num> m_colors = 
	{Color(50, 0, 0), Color(100, 0, 0), Color(150, 0, 0), Color(200, 0, 0), Color(250, 0, 0)};
	const std::size_t change_color_particles_num = 5U;
};

int main()
{
	Visualizer_Brownian_Motion visualizer;

	visualizer.start_motion();

	system("pause");

	return 0;
}