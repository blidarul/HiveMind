#include "map.h"
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>

//MAP CLASS IMPLEMENTATION

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

	m_data.resize(static_cast<size_t>(m_width) * static_cast<size_t>(m_height)); // resize vector to avoid reallocations

	for (size_t i = 0; i < m_data.size(); ++i)
	{
		// Initialize all cells to '.' (empty grid)
		m_data[i] = CellType::empty;
	}
}

Map::~Map() {}

// getters

int Map::getWidth() const
{
	return m_width;
}

int Map::getHeight() const
{
	return m_height;
}

std::vector<CellType>& Map::data()
{
	return m_data;
}

void Map::printMap() const // debug function
{
	for (int y = 0; y < m_height; ++y)
	{
		for (int x = 0; x < m_width; ++x)
		{
			size_t index = static_cast<size_t>(y) * static_cast<size_t>(m_width) + static_cast<size_t>(x);
			std::cout << (char)m_data[index] << ' '; // print cells with spaces between them for better visibility
		}
		std::cout << '\n';
	}
}

//MAP GENERATION CODE (FILE MAP LOADER / PROCEDURAL MAP GENERATOR) IMPLEMENTATION

void FileMapLoader::generate(Map& map)
{
	std::ifstream file(DEBUG_MAP_FILE_PATH);
	if (!file.is_open())
	{
		throw std::runtime_error("Failed to open map file");
	}
	
	for (int y = 0; y < map.getHeight(); ++y)
	{
		for (int x = 0; x < map.getWidth(); ++x)
		{
			char cellChar;
			file >> cellChar;
			size_t index = static_cast<size_t>(y) * static_cast<size_t>(map.getWidth()) + static_cast<size_t>(x);
			switch (cellChar)
			{
			case CellType::empty:
			case CellType::wall:
			case CellType::base:
			case CellType::station:
			case CellType::destination:
				map.data()[index] = static_cast<CellType>(cellChar);
				break;
			default:
				throw std::runtime_error("Invalid character in map file");
			}
		}
	}
}

void ProceduralMapGenerator::generate(Map& map)
{
	for (int y = 0; y < map.getHeight(); ++y)
	{
		for (int x = 0; x < map.getWidth(); ++x)
		{
			size_t index = static_cast<size_t>(y) * static_cast<size_t>(map.getWidth()) + static_cast<size_t>(x);
			if (x == 0 || y == 0 || x == map.getWidth() - 1 || y == map.getHeight() - 1)
			{
				map.data()[index] = CellType::wall;
			}
			else
			{
				map.data()[index] = CellType::empty;
			}
		}
	}
}

//MAP GENERATOR CLASS IMPLEMENTATION

MapGenerator::MapGenerator(IMapGenerator* generator) : m_generator(generator) {}

void MapGenerator::setGenerator(IMapGenerator* generator)
{
	m_generator = generator;
}

void MapGenerator::generateMap(Map& map)
{
	if (m_generator)
	{
		m_generator->generate(map);
	}
	else
	{
		throw std::runtime_error("No map generator set");
	}
}