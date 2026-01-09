#pragma once
#include "constants.h"
#include <vector>

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
	size_t getIndex(int x, int y) const;
	mapPosition getHubPosition() const;
	std::vector<mapPosition> getStationPositions() const;
	std::vector<mapPosition> getClientPositions() const;
	size_t getStationCount() const;
	size_t getClientCount() const;
	const std::vector<int>& getFloodfillData() const;

	void computeFloodfill(int startX, int startY);
	
	void printMap() const;
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
	std::vector<int> floodfill(int startX, int startY) const;

	int m_width;
	int m_height;
	std::vector<CellType> m_data;
	std::vector<int> m_floodfillData;

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
	bool verifyMap(const Map& map) const;

private:
	IMapGenerator *m_generator;
};