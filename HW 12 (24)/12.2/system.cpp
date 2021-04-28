#include "system.hpp"

void System::initialize()
{
	const auto size_1 = std::size(m_particles);
	const auto size_2 = std::size(m_particles[0]);

	const auto stiffness = 0.5f;

	for (auto i = 0U; i < size_1; ++i)
	{
		for (auto j = 0U; j < size_2; ++j)
		{
			if (j > 0)
			{
				m_links.push_back(Link(particle(i, j), particle(i, j - 1), stiffness));
			}
			if (i > 0)
			{
				m_links.push_back(Link(particle(i, j), particle(i - 1, j), stiffness));
				if (j > 0)
				{
					m_links.push_back(Link(particle(i, j), particle(i - 1, j - 1), stiffness));
				}
				if (j + 1 < size_2)
				{
					m_links.push_back(Link(particle(i, j), particle(i - 1, j + 1), stiffness));
				}
			}
		}
	}
}

void System::push(const sf::Vector2f force) const
{
	for (auto i = 0U; i < std::size(m_particles); ++i)
	{
		for (auto j = 0U; j < std::size(m_particles[i]); ++j)
		{
			m_particles[i][j]->move(force);
		}
	}
}

void System::update() const
{
	for (auto i = 0U; i < std::size(m_particles); ++i)
	{
		for (auto j = 0U; j < std::size(m_particles[i]); ++j)
		{
			m_particles[i][j]->move(0.25f);

			if (m_particles[i][j]->position().y + m_particles[i][j]->radius() > m_max_point.y)
			{
				m_particles[i][j]->set_y(m_max_point.y - m_particles[i][j]->radius());
			}

			if (m_particles[i][j]->position().y - m_particles[i][j]->radius() < m_min_point.y)
			{
				m_particles[i][j]->set_y(m_min_point.y + m_particles[i][j]->radius());
			}

			if (m_particles[i][j]->position().x + m_particles[i][j]->radius() > m_max_point.x)
			{
				m_particles[i][j]->set_x(m_max_point.x - m_particles[i][j]->radius());
			}

			if (m_particles[i][j]->position().x - m_particles[i][j]->radius() < m_min_point.x)
			{
				m_particles[i][j]->set_x(m_min_point.x + m_particles[i][j]->radius());
			}
		}
	}

	for (auto i = 0U; i < std::size(m_links); ++i)
	{
		m_links[i].update();
	}
}