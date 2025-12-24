#pragma once
#include <vector>

constexpr auto DEBUG_MAP_FILE_PATH = "debug_map.txt";

enum CellType
{
	empty = '.',
	wall = '#',
	base = 'B',
	station = 'S',
	destination = 'D'
};

class Map
{
public:
	Map(int width, int height);
	~Map();
	int getWidth() const;
	int getHeight() const;
	void printMap() const;
	std::vector<CellType>& data();
private:
	int m_width;
	int m_height;
	std::vector<CellType> m_data;
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
	IMapGenerator *m_generator;
};