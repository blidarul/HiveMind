#include <iostream>
#include "map.h"
#include "map.cpp"

int main()
{
	try
	{
		Map map(20, 20);

		map.printMap();

		map.generateMap();
		map.printMap();
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}
}
