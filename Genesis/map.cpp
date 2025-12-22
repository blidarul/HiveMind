#include "map.h"
#include <iostream>
#include <stdexcept>
#include <vector>

Map::Map(int width, int height) : m_width(width), m_height(height) 
{
	if (width < 0 || height < 0)
	{
		throw std::invalid_argument("Map dimensions must be non-negative");
	}
	if (width > 0 && height > 0)
	{
		if (width > 100000 / height)
		{
			throw std::overflow_error("Map size too large");
		}
	}

	m_data.resize(static_cast<size_t>(m_width) * static_cast<size_t>(m_height));

	for (size_t i = 0; i < m_data.size(); ++i)
	{
		// Initialize all cells to '.' (empty grid)
		m_data[i] = CellType::empty;
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
			size_t index = static_cast<size_t>(y) * static_cast<size_t>(m_width) + static_cast<size_t>(x);
			if (x == 0 || y == 0 || x == m_width - 1 || y == m_height - 1)
			{
				m_data[index] = CellType::wall;
			}
			else
			{
				m_data[index] = CellType::empty;
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
			size_t index = static_cast<size_t>(y) * static_cast<size_t>(m_width) + static_cast<size_t>(x);
			std::cout << (char)m_data[index] << ' ';
		}
		std::cout << '\n';
	}
}