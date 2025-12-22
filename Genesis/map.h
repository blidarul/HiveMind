#pragma once
#include <vector>

class Map
{
public:
	Map(int width, int height);
	~Map();
	int getWidth() const;
	int getHeight() const;
	void generateMap();
	void printMap() const;
private:
	int m_width;
	int m_height;
	std::vector<char> m_data;
};