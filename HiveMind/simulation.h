#pragma once
#include <vector>
#include "map.h"
#include <memory>
#include "agent.h"

#define	DEBUG 1

class Simulation
{
public:
	Simulation();
	~Simulation();

#ifdef DEBUG
	void printParameters() const;
	void printStatus() const;
	void printMap() const;
	void printFloodfill() const;
#endif

	void run();

private:
	void getMap();
	void initializeAgents();
	void calculateProfit();

	void advanceTick();

	//map
	Map m_map;

	//time related parameters
	int m_maxTicks;
	int m_currentTick;

	//agent related parameters
	std::vector<std::unique_ptr<IAgent>> m_agents;
	int m_agentCount;
	int m_droneCount;
	int m_robotCount;
	int m_scooterCount;

	// profit related parameters
	int m_totalRewards;
	int m_totalOperationalCosts;
	int m_totalPenalties;
	int m_profit;

	//package related parameters
	int m_totalPackages;
	int m_spawnRate;
};