#include "constants.h"
#include "map.h"
#include <fstream>
#include <iostream>
#include <random>
#include <stdexcept>
#include <vector>

//MAP CLASS IMPLEMENTATION --------------------------------

Map::Map(int width, int height, int stationCount, int clientCount)
	: m_width(width)
	, m_height(height)
	, m_stationCount(stationCount)
	, m_clientCount(clientCount)
	, m_hubPosition{ -1, -1 }
{
	if (width < 0 || height < 0)
	{
		throw std::invalid_argument("Map dimensions must be non-negative");
	}
	if (width > 0 && height > 0)
	{
		if (width > MAX_MAP_SIZE / height)
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

// getters ------------------------------------------------

int Map::getWidth() const
{
	return m_width;
}

int Map::getHeight() const
{
	return m_height;
}

size_t Map::getIndex(int x, int y) const
{
	if (x < 0 || x >= m_width || y < 0 || y >= m_height)
	{
		throw std::out_of_range("Coordinates out of map bounds");
	}
	return static_cast<size_t>(y) * static_cast<size_t>(m_width) + static_cast<size_t>(x);
}

mapPosition Map::getHubPosition() const
{
	return m_hubPosition;
}

std::vector<mapPosition> Map::getStationPositions() const
{
	return m_stationPositions;
}

std::vector<mapPosition> Map::getClientPositions() const
{
	return m_clientPositions;
}

size_t Map::getClientCount() const
{
	return getClientPositions().size();
}

size_t Map::getStationCount() const
{
	return getStationPositions().size();
}

// floodfill computation and storage ----------------------

void Map::computeFloodfill(int startX, int startY)
{
	m_floodfillData = floodfill(startX, startY);
}

const std::vector<int>& Map::getFloodfillData() const
{
	return m_floodfillData;
}

// print funtions -----------------------------------------

void Map::printMap() const // debug function
{
	for (int y = 0; y < m_height; ++y)
	{
		for (int x = 0; x < m_width; ++x)
		{
			size_t index = getIndex(x, y);
			std::cout << (char)m_data[index] << ' '; // print cells with spaces between them for better visibility
		}
		std::cout << '\n';
	}
}

#ifdef DEBUG
void Map::printFloodfill(const std::vector<int>& distances) const // debug function
{
	for (int y = 0; y < m_height; ++y)
	{
		for (int x = 0; x < m_width; ++x)
		{
			size_t index = getIndex(x, y);
			if (distances[index] == -1)
			{
				std::cout << "## "; // unreachable cells
			}
			else
			{
				std::cout << (distances[index] < 10 ? " " : "") << distances[index] << ' '; // align single digit numbers
			}
		}
		std::cout << '\n';
	}
}
#endif

// floodfill ----------------------------------------------

std::vector<int> Map::floodfill(int startX, int startY) const
{
	if (startX < 0 || startX >= m_width || startY < 0 || startY >= m_height)
	{
		throw std::out_of_range("Start coordinates out of map bounds");
	}

	//initialize vectors for cells
	std::vector<int> distances(m_width * m_height, -1);
	std::vector<bool> visited(m_width * m_height, false);

	//initialize starting point
	distances[getIndex(startX, startY)] = 0;
	visited[getIndex(startX, startY)] = true;

	//initialize "queue" for BFS
	std::vector<mapPosition> queue;
	queue.push_back({ startX, startY });
	size_t index = 0;

	while (index < queue.size())
	{
		mapPosition current = queue[index++];
		int currentDistance = distances[getIndex(current.x, current.y)];
		const std::vector<mapPosition> directions = { {0,1}, {1,0}, {0,-1}, {-1,0} }; // get the possible moving directions (no diagonals)
		for (const auto& dir : directions)
		{
			int newX = current.x + dir.x;
			int newY = current.y + dir.y;
			if (newX >= 0 && newX < m_width && newY >= 0 && newY < m_height) //check bounds
			{
				size_t neighborIndex = getIndex(newX, newY);
				if (!visited[neighborIndex] && m_data[neighborIndex] != CellType::wall)
				{
					visited[neighborIndex] = true;
					distances[neighborIndex] = currentDistance + 1;
					queue.push_back({ newX, newY });
				}
			}
		}
	}

	return distances;
}

//MAP GENERATION CODE (FILE MAP LOADER / PROCEDURAL MAP GENERATOR) IMPLEMENTATION -----------------

void FileMapLoader::generate(Map& map)
{
	// Clear previous generation data
	map.m_hubPosition = { -1, -1 };
	map.m_stationPositions.clear();
	map.m_clientPositions.clear();
	
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
			size_t index = map.getIndex(x, y);
			switch (cellChar)
			{
			case CellType::base:
				map.m_hubPosition = { x, y };
				map.m_data[index] = CellType::base;
				break;
			case CellType::station:
				map.m_stationPositions.push_back({ x, y });
				map.m_data[index] = CellType::station;
				break;
			case CellType::destination:
				map.m_clientPositions.push_back({ x, y });
				map.m_data[index] = CellType::destination;
				break;
			case CellType::wall:
			case CellType::empty:
				map.m_data[index] = static_cast<CellType>(cellChar);
				break;
			default:
				throw std::runtime_error("Invalid character in map file");
			}
		}
	}
}

// --------------------------------------------------------

void ProceduralMapGenerator::generate(Map& map)
{
	// clean sheet
	map.m_hubPosition = { -1, -1 };
	map.m_stationPositions.clear();
	map.m_clientPositions.clear();
	
	// initialize random number generator
	std::random_device rd;
	std::mt19937 gen(rd());
	
	// walls on the sides
	for (int y = 0; y < map.getHeight(); ++y)
	{
		for (int x = 0; x < map.getWidth(); ++x)
		{
			size_t index = map.getIndex(x, y);
			if (x == 0 || y == 0 || x == map.getWidth() - 1 || y == map.getHeight() - 1)
			{
				map.m_data[index] = CellType::wall;
			}
			else
			{
				map.m_data[index] = CellType::empty;
			}
		}
	}

	//hub in the center
	int centerX = map.getWidth() / 2;
	int centerY = map.getHeight() / 2;
	map.m_hubPosition = { centerX, centerY };
	size_t hubIndex = map.getIndex(map.m_hubPosition.x, map.m_hubPosition.y);
	map.m_data[hubIndex] = CellType::base;

	//random walls
	int interiorCells = (map.getWidth() - 2) * (map.getHeight() - 2);
	int wallCount = interiorCells / 4; // 1/4 of empty cells = walls
	
	std::uniform_int_distribution<> distX(1, map.getWidth() - 2);
	std::uniform_int_distribution<> distY(1, map.getHeight() - 2);
	
	for (int i = 0; i < wallCount; ++i)
	{
		int x, y;
		size_t index;
		do
		{
			//get random coordinates in the boundary
			x = distX(gen);
			y = distY(gen);

			index = map.getIndex(x, y);
		} while (map.m_data[index] != CellType::empty);
		
		map.m_data[index] = CellType::wall;
	}

	// charging stations
	for (int i = 0; i < map.m_stationCount; ++i)
	{
		int x, y;
		size_t index;
		do
		{
			x = distX(gen);
			y = distY(gen);
			index = map.getIndex(x, y);
		} while (map.m_data[index] != CellType::empty);

		map.m_stationPositions.push_back({ x, y });
		map.m_data[index] = CellType::station;
	}

	// clients
	for (int i = 0; i < map.m_clientCount; ++i)
	{
		int x, y;
		size_t index;
		do
		{
			x = distX(gen);
			y = distY(gen);
			index = map.getIndex(x, y);
		} while (map.m_data[index] != CellType::empty);
		
		map.m_clientPositions.push_back({ x, y });
		map.m_data[index] = CellType::destination;
	}
}

//MAP GENERATOR CLASS IMPLEMENTATION ----------------------

//strategy pattern implementation

MapGenerator::MapGenerator(IMapGenerator* generator) 
	: m_generator(generator) 
	{}

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

// map verification function ------------------------------

bool MapGenerator::verifyMap(const Map& map) const
{
	mapPosition hubPos = map.getHubPosition();
	if (hubPos.x == -1 && hubPos.y == -1)
	{
		return false;
	}

	int stationCount = 0;
	int destinationCount = 0;
	
	for(int y = 0; y < map.getHeight(); ++y)
	{
		for (int x = 0; x < map.getWidth(); ++x)
		{
			size_t index = map.getIndex(x, y);
			CellType cell = map.m_data[index];
			if (cell == CellType::station)
			{
				stationCount++;
			}
			else if (cell == CellType::destination)
			{
				destinationCount++;
			}
		}
	}

	if(stationCount != map.m_stationCount || destinationCount != map.m_clientCount)
	{
		return false;
	}


	// floodfill from hub
	std::vector<int> distances = map.floodfill(hubPos.x, hubPos.y);

	// check if all clients are reacheable
	for (size_t i = 0; i < map.m_clientPositions.size(); ++i)
	{
		mapPosition clientPos = map.m_clientPositions[i];
		size_t clientIndex = map.getIndex(clientPos.x, clientPos.y);
		if (distances[clientIndex] == -1)
		{
			return false;
		}
	}

	// check if all stations are reacheable
	for (size_t i = 0; i < map.m_stationPositions.size(); ++i)
	{
		mapPosition stationPos = map.m_stationPositions[i];
		size_t stationIndex = map.getIndex(stationPos.x, stationPos.y);
		if (distances[stationIndex] == -1)
		{
			return false;
		}
	}

	return true;
}