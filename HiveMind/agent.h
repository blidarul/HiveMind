#pragma once
#include "map.h"

// enums --------------------------------------------------
enum AgentSymbol
{
	DRONE = '^',
	ROBOT = 'R',
	SCOOTER = 's'
};

enum AgentState
{
	DEAD = 0,
	IDLE,
	MOVING,
	CHARGING
};

// interface class ----------------------------------------

class IAgent
{
public:
	virtual int getID() const = 0;
	virtual AgentSymbol getSymbol() const = 0;
	virtual AgentState getState() const = 0;
	virtual mapPosition getCurrentPosition() const = 0;

	virtual void move() = 0;
	virtual void chargeBattery() = 0;

	virtual ~IAgent() = default;
};

// actual agent classes -----------------------------------

class Drone : public IAgent
{
public:
	Drone(mapPosition startPos);

	int getID() const override;
	AgentSymbol getSymbol() const override;
	AgentState getState() const override;
	mapPosition getCurrentPosition() const override;

	// TO DO: implement getPath function

	void handleState();

private:
	void chargeBattery() override;
	void move() override;
	
	static int s_nextID;
	int m_id;
	AgentSymbol m_symbol;
	AgentState m_state;
	int m_ticksAlive;

	//TO DO: implement next moves vector
	mapPosition m_currentPosition;
	mapPosition m_targetPosition;
	int m_speed;

	//TO DO: implement package vector
	int m_maxCapacity;
	int m_currentLoad;

	int m_maxBattery;
	int m_currentBattery;
	int m_batteryConsumptionRate;

	int m_operationCost;
	int m_operationCostTotal;
};

// --------------------------

class Robot : public IAgent
{
public:
	Robot(mapPosition startPos);

	int getID() const override;
	AgentSymbol getSymbol() const override;
	AgentState getState() const override;
	mapPosition getCurrentPosition() const override;

	// TO DO: implement getPath function

	void handleState();

private:
	void move() override;
	void chargeBattery() override;

	static int s_nextID;
	int m_id;
	AgentSymbol m_symbol;
	AgentState m_state;
	int m_ticksAlive;

	// TO DO: implement next moves vector
	mapPosition m_currentPosition;
	mapPosition m_targetPosition;
	int m_speed;

	//TO DO: implement package vector
	int m_maxCapacity;
	int m_currentLoad;

	int m_maxBattery;
	int m_currentBattery;
	int m_batteryConsumptionRate;

	int m_operationCost;
	int m_operationCostTotal;
};

// --------------------------

class Scooter : public IAgent
{
public:
	Scooter(mapPosition startPos);

	int getID() const override;
	AgentSymbol getSymbol() const override;
	AgentState getState() const override;
	mapPosition getCurrentPosition() const override;

	//TO DO: implement getPath function
	
	void handleState();

private:
	void move() override;
	void chargeBattery() override;

	static int s_nextID;
	int m_id;
	AgentSymbol m_symbol;
	AgentState m_state;
	int m_ticksAlive;

	//TO DO: implement next moves vector
	mapPosition m_currentPosition;
	mapPosition m_targetPosition;
	int m_speed;

	//TO DO: implement package vector
	int m_maxCapacity;
	int m_currentLoad;

	int m_maxBattery;
	int m_currentBattery;
	int m_batteryConsumptionRate;

	int m_operationCost;
	int m_operationCostTotal;
};