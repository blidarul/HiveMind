#pragma once
#include <vector>
#include "constants.h"

enum CellType
{
	empty = '.',
	wall = '#',
	base = 'B',
	station = 'S',
	destination = 'D'
};

struct mapPosition
{
	int x;
	int y;
};

class Map
{
public:
	Map(int width, int height, int stationCount, int clientCount);
	~Map();
	int getWidth() const;
	int getHeight() const;
	void printMap() const;
	mapPosition getHubPosition() const;
	size_t getIndex(int x, int y) const;
	std::vector<int> floodfill(int startX, int startY) const;
#ifdef DEBUG
	void printFloodfill(const std::vector<int>& distances) const;
#endif
protected:
	mapPosition m_hubPosition;
	std::vector<mapPosition> m_stationPositions;
	std::vector<mapPosition> m_clientPositions;
	int m_stationCount;
	int m_clientCount;
private:
	int m_width;
	int m_height;
	std::vector<CellType> m_data;
friend class Simulation;
friend class MapGenerator;
friend class FileMapLoader;
friend class ProceduralMapGenerator;
};

class IMapGenerator
{
public:
	virtual ~IMapGenerator() = default;
	virtual void generate(Map& map) = 0;
};

class FileMapLoader : public IMapGenerator
{
public:
	void generate(Map& map) override;
};

class ProceduralMapGenerator : public IMapGenerator
{
public:
	void generate(Map& map) override;
};

class MapGenerator
{
public:
	explicit MapGenerator(IMapGenerator* generator);
	void setGenerator(IMapGenerator* generator);
	void generateMap(Map& map);
private:
	bool verifyMap(const Map& map) const;
	IMapGenerator *m_generator;
};