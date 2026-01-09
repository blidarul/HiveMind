#pragma once
#include "map.h"

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

class IAgent
{
public:
	virtual void moveTo(mapPosition position) = 0;
	virtual void chargeBattery() = 0;
	virtual int getID() const = 0;
	virtual AgentSymbol getSymbol() const = 0;
	virtual AgentState getState() const = 0;
	virtual mapPosition getCurrentPosition() const = 0;
	virtual ~IAgent() = default;
};

class Drone : public IAgent
{
public:
	Drone(mapPosition startPos);
	void moveTo(mapPosition position) override;
	void chargeBattery() override;
	int getID() const override;
	AgentSymbol getSymbol() const override;
	AgentState getState() const override;
	mapPosition getCurrentPosition() const override;
protected:
	static int s_nextID;
	int m_id;
	AgentSymbol m_symbol;
	AgentState m_state;
	int m_ticksAlive;

	mapPosition m_currentPosition;
	mapPosition m_targetPosition;
	int m_speed;

	int m_maxCapacity;
	int m_currentLoad;

	int m_maxBattery;
	int m_currentBattery;
	int m_batteryConsumptionRate;

	int m_operationCost;
	int m_operationCostTotal;
friend class Simulation;
};

class Robot : public IAgent
{
public:
	Robot(mapPosition startPos);
	void moveTo(mapPosition position) override;
	void chargeBattery() override;
	int getID() const override;
	AgentSymbol getSymbol() const override;
	AgentState getState() const override;
	mapPosition getCurrentPosition() const override;
protected:
	static int s_nextID;
	int m_id;
	AgentSymbol m_symbol;
	AgentState m_state;
	int m_ticksAlive;

	mapPosition m_currentPosition;
	mapPosition m_targetPosition;
	int m_speed;

	int m_maxCapacity;
	int m_currentLoad;

	int m_maxBattery;
	int m_currentBattery;
	int m_batteryConsumptionRate;

	int m_operationCost;
	int m_operationCostTotal;
friend class Simulation;
};

class Scooter : public IAgent
{
public:
	Scooter(mapPosition startPos);
	void moveTo(mapPosition position) override;
	void chargeBattery() override;
	int getID() const override;
	AgentSymbol getSymbol() const override;
	AgentState getState() const override;
	mapPosition getCurrentPosition() const override;
protected:
	static int s_nextID;
	int m_id;
	AgentSymbol m_symbol;
	AgentState m_state;
	int m_ticksAlive;

	mapPosition m_currentPosition;
	mapPosition m_targetPosition;
	int m_speed;

	int m_maxCapacity;
	int m_currentLoad;

	int m_maxBattery;
	int m_currentBattery;
	int m_batteryConsumptionRate;

	int m_operationCost;
	int m_operationCostTotal;
friend class Simulation;
};