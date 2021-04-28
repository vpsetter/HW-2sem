#include <cmath>
#include <vector>

#include <SFML/Graphics.hpp>

#include "functions.hpp"
#include "system.hpp"

int main(int argc, char ** argv)
{
	sf::RenderWindow window(sf::VideoMode(800U, 600U), "PHYSICS");

	sf::Vector2f min_point(  0.0f,   0.0f);
	sf::Vector2f max_point(775.0f, 575.0f);

	const auto N_raw = 20U;
	const auto N_column = 10U;

	const auto distance_0 = length(max_point - min_point) * 0.4f / N_raw;

	const auto r = 2.5f;

	auto position = (max_point - min_point) * 0.5f - sf::Vector2f(N_raw * distance_0 / 2, N_column * distance_0 / 2);

	std::vector <std::vector< System::particle_t >> particles;
	for (auto i = 0U; i < N_raw; ++i, position.x += distance_0)
	{
		std::vector< System::particle_t > particles_column;
		auto current_position = position;
		for (auto j = 0U; j < N_column; ++j)
		{
			particles_column.push_back(std::make_shared < Particle >(current_position, current_position,
				sf::Vector2f(0.0f, 10.0f), r));
			current_position.y += distance_0;
		}
		particles.push_back(particles_column);
	}

	System system(min_point, max_point, particles);

	sf::Event event;

	while (window.isOpen())
	{
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				window.close();
			}
		}

		system.update();

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
		{
			system.push(sf::Vector2f(0.0f, -2.0f));
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
		{
			system.push(sf::Vector2f(0.0f, 2.0f));
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
		{
			system.push(sf::Vector2f(-2.0f, 0.0f));
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
		{
			system.push(sf::Vector2f(2.0f, 0.0f));
		}

		window.clear();
		
		for (auto i = 0U; i < N_raw; ++i)
		{
			for (auto j = 0U; j < N_column; ++j)
			{
				sf::CircleShape circle(2.0f * r);

				circle.setPosition(system.particle(i, j)->position() + sf::Vector2f(r, r));

				circle.setFillColor(sf::Color::Red);

				window.draw(circle);
			}
		}
	
		window.display();
	}

	return EXIT_SUCCESS;
}
