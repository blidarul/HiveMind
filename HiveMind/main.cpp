#include "map.h"
#include <exception>
#include <iostream>

int main()
{
	try
	{
		Map map(20, 20);

		MapGenerator generator(new FileMapLoader());
		generator.generateMap(map);

		map.printMap();

	}
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}
}
