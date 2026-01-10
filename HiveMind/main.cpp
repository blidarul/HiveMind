#include "simulation.h"
#include <exception>
#include <iostream>


int main()
{
	try
	{
		std::cout << "=================================\n";
		std::cout << "    HIVEMIND SIMULATION \n";
		std::cout << "=================================\n\n";
		
		Simulation simulation;
		
#ifdef DEBUG
		std::cout << "--- MAP LAYOUT ---\n";
		simulation.printMap();
		std::cout << "\n--- FLOODFILL DISTANCES FROM HUB ---\n";
		simulation.printFloodfill();
		std::cout << "\n";
		
		simulation.printParameters();
		std::cout << "\n--- INITIAL AGENT STATUS ---\n";
		simulation.printStatus();
		std::cout << "\n";
#endif
		
		std::cout << "Starting simulation...\n\n";
		simulation.run();
		
		std::cout << "\n============================================\n";
		std::cout << "         SIMULATION COMPLETE\n";
		std::cout << "============================================\n";
	}
	catch (const std::exception& e)
	{
		std::cerr << "\n[ERROR] " << e.what() << std::endl;
		return 1;
	}
	
	return 0;
}
