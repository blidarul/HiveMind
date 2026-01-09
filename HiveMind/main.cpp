#include "simulation.h"
#include <exception>
#include <iostream>


int main()
{
	try
	{
		Simulation simulation;
#ifdef DEBUG
		simulation.printMap();
		simulation.printFloodfill();
		simulation.printParameters();
		simulation.printStatus();
#endif
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}
}
