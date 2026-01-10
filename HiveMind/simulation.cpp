#include "agent.h"
#include "map.h"
#include "simulation.h"
#include "hivemind.h"
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
	, m_packagesSpawned(0)
	, m_spawnRate(0)
	, m_ticksSinceLastSpawn(0)
	, m_totalPenalties(0)
	, m_totalRewards(0)
	, m_totalOperationalCosts(0)
	, m_profit(0)
	, m_hiveMind(nullptr)
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
	
	m_hiveMind = std::make_unique<HiveMind>(m_map);
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
	std::cout << "Packages Spawned: " << m_packagesSpawned << "/" << m_totalPackages << '\n';
	std::cout << "Agents:\n";
	for (const auto& agent : m_agents)
	{
		mapPosition pos = agent->getCurrentPosition();
		std::cout << "ID: " << agent->getID()
			<< " | Type: " << static_cast<char>(agent->getSymbol())
			<< " | State: " << static_cast<int>(agent->getState())
			<< " | Position: (" << pos.x << ", " << pos.y << ")";
		
		if (agent->getSymbol() == AgentSymbol::DRONE)
		{
			Drone* drone = static_cast<Drone*>(agent.get());
			std::cout << " | Packages: " << agent->getPackages().size();
		}
		
		std::cout << "\n";
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

void Simulation::printAlivePackages() const
{
	std::cout << "\n=== Tick " << m_currentTick << " | Packages Spawned: " << m_packagesSpawned << "/" << m_totalPackages << " ===\n";
	
	std::cout << "Packages in Base (" << m_packagesInBase.size() << "):\n";
	for (const auto& package : m_packagesInBase)
	{
		mapPosition dest = package->getDestination();
		std::cout << "  Destination: (" << dest.x << ", " << dest.y << ")"
			<< " | Remaining Ticks: " << package->getRemainingTicks()
			<< " | Reward: " << package->getReward()
			<< (package->getIsLate() ? " | [LATE]" : "") << '\n';
	}

	int totalPackagesOnAgents = 0;
	for (const auto& agent : m_agents)
	{
		totalPackagesOnAgents += static_cast<int>(agent->getPackages().size());
	}
	
	std::cout << "Packages on Agents (" << totalPackagesOnAgents << "):\n";
	for (const auto& agent : m_agents)
	{
		if (agent->getPackages().empty())
			continue;
		std::cout << "  Agent: " << static_cast<char>(agent->getSymbol()) << " ID: " << agent->getID() << '\n';
		for (const auto& package : agent->getPackages())
		{
			mapPosition dest = package->getDestination();
			std::cout << "    Destination: (" << dest.x << ", " << dest.y << ")"
				<< " | Remaining Ticks: " << package->getRemainingTicks()
				<< " | Reward: " << package->getReward()
				<< (package->getIsLate() ? " | [LATE]" : "") << '\n';
		}
	}
	
	std::cout << "Total Active Packages: " << (m_packagesInBase.size() + totalPackagesOnAgents) << "\n";
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
	m_ticksSinceLastSpawn++;
	updatePackageTicks();
}

void Simulation::updateAgents()
{
	for (auto& agent : m_agents)
	{
		int costBefore = 0;
		
		if (agent->getSymbol() == AgentSymbol::DRONE)
		{
			Drone* drone = static_cast<Drone*>(agent.get());
			costBefore = drone->getTotalOperationCost();
			drone->handleState(m_map);
			m_totalOperationalCosts += (drone->getTotalOperationCost() - costBefore);
		}
		else if (agent->getSymbol() == AgentSymbol::ROBOT)
		{
			Robot* robot = static_cast<Robot*>(agent.get());
			costBefore = robot->getTotalOperationCost();
			robot->handleState(m_map);
			m_totalOperationalCosts += (robot->getTotalOperationCost() - costBefore);
		}
		else if (agent->getSymbol() == AgentSymbol::SCOOTER)
		{
			Scooter* scooter = static_cast<Scooter*>(agent.get());
			costBefore = scooter->getTotalOperationCost();
			scooter->handleState(m_map);
			m_totalOperationalCosts += (scooter->getTotalOperationCost() - costBefore);
		}
	}
}

void Simulation::run()
{
	#ifdef DEBUG
	std::cout << "=== STARTING HIVEMIND SIMULATION ===\n";
	printParameters();
	std::cout << "\n";
	#endif
	
	int packagesAssigned = 0;
	int packagesDelivered = 0;
	
	while (m_currentTick < m_maxTicks)
	{
		advanceTick();
		spawnPackage();
		
		if (m_hiveMind && !m_packagesInBase.empty())
		{
			size_t beforeAssignment = m_packagesInBase.size();
			m_hiveMind->assignPackages(m_packagesInBase, m_agents);
			size_t afterAssignment = m_packagesInBase.size();
			
			int justAssigned = static_cast<int>(beforeAssignment - afterAssignment);
			if (justAssigned > 0)
			{
				packagesAssigned += justAssigned;
				#ifdef DEBUG
				std::cout << "[Tick " << m_currentTick << "] HiveMind assigned " 
					<< justAssigned << " package(s) to agents.\n";
				#endif
			}
		}
		
		updateAgents();
		
		for (auto& agent : m_agents)
		{
			if (agent->getSymbol() == AgentSymbol::DRONE)
			{
				Drone* drone = static_cast<Drone*>(agent.get());
			}
		}
		
		#ifdef DEBUG
		if (m_currentTick % 10 == 0)
		{
			printAlivePackages();
		}
		#endif
	}
	
	calculatePenalties();
	calculateProfit();
	
	generateReport();
	
	#ifdef DEBUG
	std::cout << "\n=== SIMULATION COMPLETE ===\n";
	std::cout << "Total Packages Assigned: " << packagesAssigned << "\n";
	std::cout << "Total Rewards: $" << m_totalRewards << "\n";
	std::cout << "Total Operational Costs: $" << m_totalOperationalCosts << "\n";
	std::cout << "Total Penalties: $" << m_totalPenalties << "\n";
	std::cout << "Final Profit: $" << m_profit << "\n";
	std::cout << "Report saved to simulation.txt\n";
	#endif
}

void Simulation::calculateProfit()
{
	m_profit = m_totalRewards - m_totalOperationalCosts - m_totalPenalties;
}

void Simulation::calculatePenalties()
{
	int deadAgents = 0;
	for (const auto& agent : m_agents)
	{
		if (agent->getState() == AgentState::DEAD)
		{
			deadAgents++;
			m_totalPenalties += DEAD_AGENT_PENALTY;
		}
	}
	
	for (auto& agent : m_agents)
	{
		if (agent->getSymbol() == AgentSymbol::DRONE)
		{
			Drone* drone = static_cast<Drone*>(agent.get());
			m_totalRewards += drone->getPersonalRewards();
		}
		else if (agent->getSymbol() == AgentSymbol::ROBOT)
		{
			Robot* robot = static_cast<Robot*>(agent.get());
			m_totalRewards += robot->getPersonalRewards();
		}
		else if (agent->getSymbol() == AgentSymbol::SCOOTER)
		{
			Scooter* scooter = static_cast<Scooter*>(agent.get());
			m_totalRewards += scooter->getPersonalRewards();
		}
	}
	
	int undeliveredPackages = static_cast<int>(m_packagesInBase.size());
	for (const auto& agent : m_agents)
	{
		undeliveredPackages += static_cast<int>(agent->getPackages().size());
	}
	
	m_totalPenalties += undeliveredPackages * PACKAGE_NOT_DELIVERED_PENALTY;
	
	#ifdef DEBUG
	std::cout << "\n--- FINAL STATISTICS ---\n";
	std::cout << "Dead Agents: " << deadAgents << " (Penalty: $" << (deadAgents * DEAD_AGENT_PENALTY) << ")\n";
	std::cout << "Undelivered Packages: " << undeliveredPackages 
		<< " (Penalty: $" << (undeliveredPackages * PACKAGE_NOT_DELIVERED_PENALTY) << ")\n";
	#endif
}

void Simulation::generateReport()
{
	std::ofstream report("simulation.txt");
	
	if (!report.is_open())
	{
		std::cerr << "Error: Could not create simulation.txt report\n";
		return;
	}
	
	report << "========================================\n";
	report << "   HIVEMIND SIMULATION REPORT\n";
	report << "========================================\n\n";
	
	report << "SIMULATION PARAMETERS:\n";
	report << "  Map Size: " << m_map.getWidth() << "x" << m_map.getHeight() << "\n";
	report << "  Total Ticks: " << m_maxTicks << "\n";
	report << "  Clients: " << m_map.getClientCount() << "\n";
	report << "  Charging Stations: " << m_map.getStationCount() << "\n\n";
	
	report << "FLEET CONFIGURATION:\n";
	report << "  Drones: " << m_droneCount << "\n";
	report << "  Robots: " << m_robotCount << "\n";
	report << "  Scooters: " << m_scooterCount << "\n";
	report << "  Total Agents: " << m_agentCount << "\n\n";
	
	report << "PACKAGE STATISTICS:\n";
	report << "  Total Packages Spawned: " << m_packagesSpawned << "\n";
	
	int deliveredPackages = 0;
	int packagesOnAgents = 0;
	for (const auto& agent : m_agents)
	{
		packagesOnAgents += static_cast<int>(agent->getPackages().size());
		if (agent->getSymbol() == AgentSymbol::DRONE)
		{
			Drone* drone = static_cast<Drone*>(agent.get());
			deliveredPackages += drone->getPersonalRewards() > 0 ? 1 : 0;
		}
		else if (agent->getSymbol() == AgentSymbol::ROBOT)
		{
			Robot* robot = static_cast<Robot*>(agent.get());
			deliveredPackages += robot->getPersonalRewards() > 0 ? 1 : 0;
		}
		else if (agent->getSymbol() == AgentSymbol::SCOOTER)
		{
			Scooter* scooter = static_cast<Scooter*>(agent.get());
			deliveredPackages += scooter->getPersonalRewards() > 0 ? 1 : 0;
		}
	}
	
	int undeliveredPackages = static_cast<int>(m_packagesInBase.size()) + packagesOnAgents;
	
	report << "  Packages Delivered: " << (m_packagesSpawned - undeliveredPackages) << "\n";
	report << "  Packages in Base: " << m_packagesInBase.size() << "\n";
	report << "  Packages on Agents: " << packagesOnAgents << "\n";
	report << "  Delivery Success Rate: " 
		<< (m_packagesSpawned > 0 ? (100.0 * (m_packagesSpawned - undeliveredPackages) / m_packagesSpawned) : 0.0) 
		<< "%\n\n";
	
	report << "AGENT STATUS:\n";
	int deadAgents = 0;
	int idleAgents = 0;
	int movingAgents = 0;
	int chargingAgents = 0;
	
	for (const auto& agent : m_agents)
	{
		switch (agent->getState())
		{
		case AgentState::DEAD: deadAgents++; break;
		case AgentState::IDLE: idleAgents++; break;
		case AgentState::MOVING: movingAgents++; break;
		case AgentState::CHARGING: chargingAgents++; break;
		}
	}
	
	report << "  Dead: " << deadAgents << "\n";
	report << "  Idle: " << idleAgents << "\n";
	report << "  Moving: " << movingAgents << "\n";
	report << "  Charging: " << chargingAgents << "\n\n";
	
	report << "========================================\n";
	report << "FINANCIAL SUMMARY:\n";
	report << "========================================\n";
	report << "  Total Rewards: $" << m_totalRewards << "\n";
	report << "  Operational Costs: -$" << m_totalOperationalCosts << "\n";
	report << "  Penalties: -$" << m_totalPenalties << "\n";
	report << "    - Dead Agents (" << deadAgents << "): -$" << (deadAgents * DEAD_AGENT_PENALTY) << "\n";
	report << "    - Undelivered Packages (" << undeliveredPackages << "): -$" 
		<< (undeliveredPackages * PACKAGE_NOT_DELIVERED_PENALTY) << "\n";
	report << "----------------------------------------\n";
	report << "  NET PROFIT: $" << m_profit << "\n";
	report << "========================================\n\n";
	
	report << "PERFORMANCE ANALYSIS:\n";
	if (m_profit > 0)
	{
		report << "  Status: PROFITABLE ?\n";
	}
	else if (m_profit == 0)
	{
		report << "  Status: BREAK EVEN\n";
	}
	else
	{
		report << "  Status: LOSS\n";
	}
	
	float profitPerTick = static_cast<float>(m_profit) / m_maxTicks;
	report << "  Profit per Tick: $" << profitPerTick << "\n";
	
	float costEfficiency = m_totalRewards > 0 ? 
		(100.0f * m_totalOperationalCosts / m_totalRewards) : 0.0f;
	report << "  Cost Efficiency: " << costEfficiency << "%\n";
	
	report << "\n========================================\n";
	report << "End of Report\n";
	report << "========================================\n";
	
	report.close();
}

void Simulation::spawnPackage()
{
	if(m_ticksSinceLastSpawn >= m_spawnRate && m_packagesSpawned < m_totalPackages)
	{
		auto newPackage = std::make_unique<Package>(m_map);
		m_packagesInBase.push_back(std::move(newPackage));
		m_ticksSinceLastSpawn = 0;
		m_packagesSpawned++;
	}
}

void Simulation::updatePackageTicks()
{
	for (auto& package : m_packagesInBase)
	{
		package->decrementTick();
	}
	
	for (auto& agent : m_agents)
	{
		for (auto& package : agent->getPackages())
		{
			package->decrementTick();
		}
	}
}
