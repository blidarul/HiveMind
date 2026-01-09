#include "agent.h"
#include "map.h"
#include "simulation.h"
#include <fstream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#ifdef DEBUG
#include <iostream>
#endif

Simulation::Simulation()
	: m_map(0, 0, 0, 0)
	, m_maxTicks(0)
	, m_currentTick(0)
	, m_agentCount(0)
	, m_droneCount(0)
	, m_robotCount(0)
	, m_scooterCount(0)
	, m_totalPackages(0)
	, m_spawnRate(0)
	, m_totalPenalties(0)
	, m_totalRewards(0)
	, m_totalOperationalCosts(0)
	, m_profit(0)
{
	std::ifstream setupFile("simulation_setup.txt");
	if (!setupFile)
	{
		throw std::runtime_error("Failed to open simulation setup file");
	}

	int width = 0, height = 0, maxStations = 0, clientCount = 0;

	std::string line;
	while (std::getline(setupFile, line))
	{
		if (line.empty())
			continue;

		std::istringstream iss(line);
		std::string key;
		std::getline(iss, key, ':');

		if (key == "MAP_SIZE")
		{
			iss >> width >> height;
		}
		else if (key == "MAX_TICKS")
		{
			iss >> m_maxTicks;
		}
		else if (key == "MAX_STATIONS")
		{
			iss >> maxStations;
		}
		else if (key == "CLIENTS_COUNT")
		{
			iss >> clientCount;
		}
		else if (key == "DRONES")
		{
			iss >> m_droneCount;
		}
		else if (key == "ROBOTS")
		{
			iss >> m_robotCount;
		}
		else if (key == "SCOOTERS")
		{
			iss >> m_scooterCount;
		}
		else if (key == "TOTAL_PACKAGES")
		{
			iss >> m_totalPackages;
		}
		else if (key == "SPAWN_FREQUENCY")
		{
			iss >> m_spawnRate;
		}
	}

	setupFile.close();

	m_map = Map(width, height, maxStations, clientCount);

	// run the map and agent initialization functions
	getMap();
	initializeAgents();
}

#ifdef DEBUG
void Simulation::printParameters() const
{
	std::cout << "Simulation Parameters:\n";
	std::cout << "Max Ticks: " << m_maxTicks << '\n';
	std::cout << "Agents: " << m_agentCount << " (Drones: " << m_droneCount
		<< ", Robots: " << m_robotCount << ", Scooters: " << m_scooterCount << ")\n";
	std::cout << "Total Packages: " << m_totalPackages << '\n';
	std::cout << "Spawn Frequency: " << m_spawnRate << '\n';
}

void Simulation::printStatus() const
{
	std::cout << "Tick: " << m_currentTick << "/" << m_maxTicks << '\n';
	std::cout << "Agents:\n";
	for (const auto& agent : m_agents)
	{
		mapPosition pos = agent->getCurrentPosition();
		std::cout << "ID: " << agent->getID()
			<< " | Type: " << static_cast<char>(agent->getSymbol())
			<< " | State: " << static_cast<int>(agent->getState())
			<< " | Position: (" << pos.x << ", " << pos.y << ")\n";
	}
}

void Simulation::printMap() const
{
	m_map.printMap();
}

void Simulation::printFloodfill() const
{
	m_map.printFloodfill(m_map.getFloodfillData());
}
#endif

Simulation::~Simulation()
{
	m_agents.clear();
}

void Simulation::getMap()
{
	MapGenerator* generator;

	#ifdef DEBUG
		generator = new MapGenerator(new FileMapLoader());
		generator->generateMap(m_map);
		if (!generator->verifyMap(m_map))
		{
			delete generator;
			throw std::runtime_error("Invalid map file: map verification failed");
		}
	#else
		generator = new MapGenerator(new ProceduralMapGenerator());
		do
		{
			generator->generateMap(m_map);
		} while (!generator->verifyMap(m_map));
	#endif
	
	delete generator;
	
	// Compute and store floodfill data from hub position
	mapPosition hubPos = m_map.getHubPosition();
	m_map.computeFloodfill(hubPos.x, hubPos.y);
}

void Simulation::initializeAgents()
{
	//get total agents number
	m_agentCount = m_scooterCount + m_robotCount + m_droneCount;

	//resize vector
	m_agents.resize(static_cast<size_t>(m_agentCount));

	//create agents at hub position
	size_t i = 0;
	for (; i < static_cast<size_t>(m_droneCount); ++i)
		m_agents[i] = std::make_unique<Drone>(m_map.getHubPosition());
	for (; i < static_cast<size_t>(m_droneCount + m_robotCount); ++i)
		m_agents[i] = std::make_unique<Robot>(m_map.getHubPosition());
	for (; i < static_cast<size_t>(m_agentCount); ++i)
		m_agents[i] = std::make_unique<Scooter>(m_map.getHubPosition());
}

void Simulation::advanceTick()
{
	m_currentTick++;
}

void Simulation::run()
{
	while (m_currentTick < m_maxTicks)
	{
		advanceTick();

	}
}

void Simulation::calculateProfit()
{
	m_profit = m_totalRewards - m_totalOperationalCosts - m_totalPenalties;
}