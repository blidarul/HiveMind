#include <limits>
#include <stdexcept>
#include <iostream>
#include "map.h"

Map::Map(int width, int height) : m_width(width), m_height(height) 
{
	if (width < 0 || height < 0)
	{
		throw std::invalid_argument("Map dimensions must be non-negative");
	}
	if (width > 0 && height > 0)
	{
		if (width > std::numeric_limits<size_t>::max() / height)
		{
			throw std::overflow_error("Map size too large");
		}
	}
	m_data.resize(static_cast<size_t>(m_width) * static_cast<size_t>(m_height));
	for (size_t i = 0; i < m_data.size(); ++i)
	{
		// Initialize all cells to '.' (empty grid)
		m_data[i] = '.';
	}
}

Map::~Map() {}

int Map::getWidth() const
{
	return m_width;
}

int Map::getHeight() const
{
	return m_height;
}

void Map::generateMap()
{
	for (int y = 0; y < m_height; ++y)
	{
		for (int x = 0; x < m_width; ++x)
		{
			if (x == 0 || y == 0 || x == m_width - 1 || y == m_height - 1)
			{
				m_data[y * m_width + x] = '#';
			}
			else
			{
				m_data[y * m_width + x] = '.';
			}
		}
	}
}

void Map::printMap() const
{
	for (int y = 0; y < m_height; ++y)
	{
		for (int x = 0; x < m_width; ++x)
		{
			std::cout << m_data[y * m_width + x] << ' ';
		}
		std::cout << '\n';
	}
}