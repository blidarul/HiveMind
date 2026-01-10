#pragma once
#include "map.h"
#include "package.h"
#include <memory>
#include <vector>

// enums --------------------------------------------------
enum AgentSymbol
{
	DRONE = '^',
	ROBOT = 'R',
	SCOOTER = 'S'
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
	virtual std::vector<std::unique_ptr<Package>>& getPackages() = 0;
	virtual int getCurrentBattery() const = 0;
	virtual int getMaxBattery() const = 0;

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
	std::vector<std::unique_ptr<Package>>& getPackages() override;
	int getCurrentBattery() const override;
	int getMaxBattery() const override;
	int getPersonalRewards() const;
	int getTotalOperationCost() const;

	void handleState(const Map& map);
	bool assignPackage(std::unique_ptr<Package> package);

private:
	bool checkDeath();
	void move() override;
	bool deliverPackage();

	void pathfindToTarget();
	void pathfindToHub(const Map& map);

	void chargeBattery() override;
	bool shouldVisitChargingStation(const Map& map) const;
	void routeToNearestChargingStation(const Map& map);

	static int s_nextID;
	int m_id;
	AgentSymbol m_symbol;
	AgentState m_state;
	int m_ticksMoving;

	std::vector<mapPosition> m_nextMoves;
	mapPosition m_currentPosition;
	mapPosition m_targetPosition;
	int m_speed;

	std::vector<std::unique_ptr<Package>> m_packages;
	int m_maxCapacity;
	int m_currentLoad;
	int m_personalRewards;

	int m_maxBattery;
	int m_currentBattery;
	int m_batteryConsumptionRate;

	int m_operationCost;
	int m_operationCostTotal;
	
	bool m_needsCharging;
	mapPosition m_chargingStationTarget;
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
	std::vector<std::unique_ptr<Package>>& getPackages() override;
	int getCurrentBattery() const override;
	int getMaxBattery() const override;
	int getTotalOperationCost() const;
	int getPersonalRewards() const;

	void handleState(const Map& map);
	bool assignPackage(std::unique_ptr<Package> package, const Map& map);

private:
	bool checkDeath();
	void move() override;
	bool deliverPackage();

	void pathfindToTarget(const Map& map);
	void pathfindToHub(const Map& map);

	void chargeBattery() override;
	bool shouldVisitChargingStation(const Map& map) const;
	void routeToNearestChargingStation(const Map& map);

	static int s_nextID;
	int m_id;
	AgentSymbol m_symbol;
	AgentState m_state;
	int m_ticksMoving;

	std::vector<mapPosition> m_nextMoves;
	mapPosition m_currentPosition;
	mapPosition m_targetPosition;
	int m_speed;

	std::vector<std::unique_ptr<Package>> m_packages;
	int m_maxCapacity;
	int m_currentLoad;
	int m_personalRewards;

	int m_maxBattery;
	int m_currentBattery;
	int m_batteryConsumptionRate;

	int m_operationCost;
	int m_operationCostTotal;
	
	bool m_needsCharging;
	mapPosition m_chargingStationTarget;
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
	std::vector<std::unique_ptr<Package>>& getPackages() override;
	int getCurrentBattery() const override;
	int getMaxBattery() const override;
	int getTotalOperationCost() const;
	int getPersonalRewards() const;
	
	void handleState(const Map& map);
	bool assignPackage(std::unique_ptr<Package> package, const Map& map);

private:
	bool checkDeath();
	void move() override;
	bool deliverPackage();

	void pathfindToTarget(const Map& map);
	void pathfindToHub(const Map& map);

	void chargeBattery() override;
	bool shouldVisitChargingStation(const Map& map) const;
	void routeToNearestChargingStation(const Map& map);

	static int s_nextID;
	int m_id;
	AgentSymbol m_symbol;
	AgentState m_state;
	int m_ticksMoving;

	std::vector<mapPosition> m_nextMoves;
	mapPosition m_currentPosition;
	mapPosition m_targetPosition;
	int m_speed;

	std::vector<std::unique_ptr<Package>> m_packages;
	int m_maxCapacity;
	int m_currentLoad;
	int m_personalRewards;

	int m_maxBattery;
	int m_currentBattery;
	int m_batteryConsumptionRate;

	int m_operationCost;
	int m_operationCostTotal;
	
	bool m_needsCharging;
	mapPosition m_chargingStationTarget;
};