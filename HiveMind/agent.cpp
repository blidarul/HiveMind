#include "agent.h"
#include "constants.h"
#include "map.h"
#include "package.h"
#include <cmath>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>

// static members -----------------------------------------
int Drone::s_nextID = 1;
int Robot::s_nextID = 1;
int Scooter::s_nextID = 1;

// ID getters ---------------------------------------------

int Drone::getID() const
{
	return m_id;
}

int Robot::getID() const
{
	return m_id;
}

int Scooter::getID() const
{
	return m_id;
}

// state getters ------------------------------------------

AgentState Drone::getState() const
{
	return m_state;
}

AgentState Robot::getState() const
{
	return m_state;
}

AgentState Scooter::getState() const
{
	return m_state;
}

// symbol getters -----------------------------------------

AgentSymbol Drone::getSymbol() const
{
	return m_symbol;
}

AgentSymbol Robot::getSymbol() const
{
	return m_symbol;
}

AgentSymbol Scooter::getSymbol() const
{
	return m_symbol;
}

// position getters ---------------------------------------

mapPosition Drone::getCurrentPosition() const
{
	return m_currentPosition;
}

mapPosition Robot::getCurrentPosition() const
{
	return m_currentPosition;
}

mapPosition Scooter::getCurrentPosition() const
{
	return m_currentPosition;
}

// battery getters ----------------------------------------

int Drone::getCurrentBattery() const
{
	return m_currentBattery;
}

int Drone::getMaxBattery() const
{
	return m_maxBattery;
}

int Robot::getCurrentBattery() const
{
	return m_currentBattery;
}

int Robot::getMaxBattery() const
{
	return m_maxBattery;
}

int Scooter::getCurrentBattery() const
{
	return m_currentBattery;
}

int Scooter::getMaxBattery() const
{
	return m_maxBattery;
}

// packages getters ---------------------------------------

std::vector<std::unique_ptr<Package>>& Drone::getPackages()
{
	return m_packages;
}

std::vector<std::unique_ptr<Package>>& Robot::getPackages()
{
	return m_packages;
}

std::vector<std::unique_ptr<Package>>& Scooter::getPackages()
{
	return m_packages;
}

// operation cost getters ---------------------------------

int Drone::getTotalOperationCost() const
{
	return m_operationCostTotal;
}

int Drone::getPersonalRewards() const
{
	return m_personalRewards;
}

int Robot::getTotalOperationCost() const
{
	return m_operationCostTotal;
}

int Robot::getPersonalRewards() const
{
	return m_personalRewards;
}

int Scooter::getTotalOperationCost() const
{
	return m_operationCostTotal;
}

int Scooter::getPersonalRewards() const
{
	return m_personalRewards;
}

// constructors -------------------------------------------

Drone::Drone(mapPosition startPos)
{
	m_id = s_nextID++;
	m_symbol = AgentSymbol::DRONE;
	m_state = AgentState::IDLE;
	m_ticksMoving = 0;

	m_targetPosition = m_currentPosition = startPos;
	m_speed = DRONE_SPEED;

	m_currentBattery = m_maxBattery = DRONE_MAX_BATTERY;
	m_batteryConsumptionRate = DRONE_BATTERY_CONSUMPTION_RATE;

	m_operationCost = DRONE_OPERATION_COST;
	m_operationCostTotal = 0;

	m_maxCapacity = DRONE_MAX_CAPACITY;
	m_currentLoad = 0;
	m_personalRewards = 0;
	
	m_needsCharging = false;
	m_chargingStationTarget = {-1, -1};
	
	#ifdef DEBUG
	std::cout << "[INIT] Drone " << m_id << " created at (" << startPos.x << ", " << startPos.y 
		<< ") - State: IDLE, Battery: " << m_currentBattery << "/" << m_maxBattery << "\n";
	#endif
}

// --------------------------

Robot::Robot(mapPosition startPos)
{
	m_id = s_nextID++;
	m_symbol = AgentSymbol::ROBOT;
	m_state = AgentState::IDLE;
	m_ticksMoving = 0;

	m_targetPosition = m_currentPosition = startPos;
	m_speed = ROBOT_SPEED;

	m_currentBattery = m_maxBattery = ROBOT_MAX_BATTERY;
	m_batteryConsumptionRate = ROBOT_BATTERY_CONSUMPTION_RATE;

	m_operationCost = ROBOT_OPERATION_COST;
	m_operationCostTotal = 0;

	m_maxCapacity = ROBOT_MAX_CAPACITY;
	m_currentLoad = 0;
	m_personalRewards = 0;
	
	m_needsCharging = false;
	m_chargingStationTarget = {-1, -1};
	
	#ifdef DEBUG
	std::cout << "[INIT] Robot " << m_id << " created at (" << startPos.x << ", " << startPos.y 
		<< ") - State: IDLE, Battery: " << m_currentBattery << "/" << m_maxBattery << "\n";
	#endif
}

//---------------------------

Scooter::Scooter(mapPosition startPos)
{
	m_id = s_nextID++;
	m_symbol = AgentSymbol::SCOOTER;
	m_state = AgentState::IDLE;
	m_ticksMoving = 0;

	m_targetPosition = m_currentPosition = startPos;
	m_speed = SCOOTER_SPEED;

	m_currentBattery = m_maxBattery = SCOOTER_MAX_BATTERY;
	m_batteryConsumptionRate = SCOOTER_BATTERY_CONSUMPTION_RATE;

	m_operationCost = SCOOTER_OPERATION_COST;
	m_operationCostTotal = 0;

	m_maxCapacity = SCOOTER_MAX_CAPACITY;
	m_currentLoad = 0;
	m_personalRewards = 0;
	
	m_needsCharging = false;
	m_chargingStationTarget = {-1, -1};
	
	#ifdef DEBUG
	std::cout << "[INIT] Scooter " << m_id << " created at (" << startPos.x << ", " << startPos.y 
		<< ") - State: IDLE, Battery: " << m_currentBattery << "/" << m_maxBattery << "\n";
	#endif
}

// move functions -----------------------------------------

void Drone::move()
{
	if (m_nextMoves.empty())
		return;

	mapPosition move = m_nextMoves.front();
	m_nextMoves.erase(m_nextMoves.begin());

	m_currentPosition.x += move.x;
	m_currentPosition.y += move.y;

}

// --------------------------

void Robot::move()
{
	if (m_nextMoves.empty())
		return;

	mapPosition move = m_nextMoves.front();
	m_nextMoves.erase(m_nextMoves.begin());

	m_currentPosition.x += move.x;
	m_currentPosition.y += move.y;

}

// --------------------------

void Scooter::move()
{
	if (m_nextMoves.empty())
		return;

	mapPosition move = m_nextMoves.front();
	m_nextMoves.erase(m_nextMoves.begin());

	m_currentPosition.x += move.x;
	m_currentPosition.y += move.y;

}

// battery charging functions -----------------------------

void Drone::chargeBattery()
{
	if (m_currentBattery < m_maxBattery)
		m_currentBattery += static_cast<int>(m_maxBattery * BATTERY_RECHARGE_RATE);
	else
		m_currentBattery = m_maxBattery;
}

void Robot::chargeBattery()
{
	if (m_currentBattery < m_maxBattery)
		m_currentBattery += static_cast<int>(m_maxBattery * BATTERY_RECHARGE_RATE);
	else
		m_currentBattery = m_maxBattery;
}

void Scooter::chargeBattery()
{
	if (m_currentBattery < m_maxBattery)
		m_currentBattery += static_cast<int>(m_maxBattery * BATTERY_RECHARGE_RATE);
	else
		m_currentBattery = m_maxBattery;
}

// check death functions ----------------------------------

bool Drone::checkDeath()
{
	if (m_currentBattery <= 0)
	{
		m_state = AgentState::DEAD;
		return true;
	}
	return false;
}

bool Robot::checkDeath()
{
	if (m_currentBattery <= 0)
	{
		m_state = AgentState::DEAD;
		return true;
	}
	return false;
}

bool Scooter::checkDeath()
{
	if (m_currentBattery <= 0)
	{
		m_state = AgentState::DEAD;
		return true;
	}
	return false;
}

// pathfinding to target ----------------------------------

void Drone::pathfindToTarget()
{
	mapPosition	calculatedPosition = m_currentPosition;

	while (m_targetPosition.x != calculatedPosition.x || m_targetPosition.y != calculatedPosition.y)
	{
		if (calculatedPosition.x < m_targetPosition.x)
		{
			m_nextMoves.push_back({ 1, 0 });
			calculatedPosition.x++;
		}
		else if (calculatedPosition.x > m_targetPosition.x)
		{
			m_nextMoves.push_back({ -1, 0 });
			calculatedPosition.x--;
		}

		if (calculatedPosition.y < m_targetPosition.y)
		{
			m_nextMoves.push_back({ 0, 1 });
			calculatedPosition.y++;
		}
		else if (calculatedPosition.y > m_targetPosition.y)
		{
			m_nextMoves.push_back({ 0, -1 });
			calculatedPosition.y--;
		}
	}
}

void Robot::pathfindToTarget(const Map& map)
{
	m_nextMoves.clear();

	// simple pathfinding
	mapPosition current = m_currentPosition;

	while (current.x != m_targetPosition.x || current.y != m_targetPosition.y)
	{
		mapPosition move = { 0, 0 };

		if (current.x < m_targetPosition.x)
			move.x = 1;
		else if (current.x > m_targetPosition.x)
			move.x = -1;
		else if (current.y < m_targetPosition.y)
			move.y = 1;
		else if (current.y > m_targetPosition.y)
			move.y = -1;

		m_nextMoves.push_back(move);
		current.x += move.x;
		current.y += move.y;
	}
}

void Scooter::pathfindToTarget(const Map& map)
{
	m_nextMoves.clear();

	// Simple greedy pathfinding - move towards target
	// This is a simplified version for ground units
	mapPosition current = m_currentPosition;

	while (current.x != m_targetPosition.x || current.y != m_targetPosition.y)
	{
		mapPosition move = { 0, 0 };

		// Prefer horizontal movement first
		if (current.x < m_targetPosition.x)
			move.x = 1;
		else if (current.x > m_targetPosition.x)
			move.x = -1;
		else if (current.y < m_targetPosition.y)
			move.y = 1;
		else if (current.y > m_targetPosition.y)
			move.y = -1;

		m_nextMoves.push_back(move);
		current.x += move.x;
		current.y += move.y;
	}
}

// pathfinding to hub -------------------------------------
	
void Drone::pathfindToHub(const Map& map)
{
	m_targetPosition = map.getHubPosition();
	pathfindToTarget();
}

void Robot::pathfindToHub(const Map& map)
{
	m_targetPosition = map.getHubPosition();
	pathfindToTarget(map);
}

void Scooter::pathfindToHub(const Map& map)
{
	m_targetPosition = map.getHubPosition();
	pathfindToTarget(map);
}

// charging station functions -----------------------------

bool Drone::shouldVisitChargingStation(const Map& map) const
{
	float batteryPercent = static_cast<float>(m_currentBattery) / m_maxBattery;
	
	if (batteryPercent < 0.4f)
		return true;
	
	// if has packages check if enough battery to deliver and return
	if (!m_packages.empty())
	{
		// estimate distance to destination and back to hub
		int distanceToDestination = std::abs(m_targetPosition.x - m_currentPosition.x) + 
									std::abs(m_targetPosition.y - m_currentPosition.y);
		int distanceToHub = std::abs(map.getHubPosition().x - m_targetPosition.x) + 
							std::abs(map.getHubPosition().y - m_targetPosition.y);
		int totalDistance = distanceToDestination + distanceToHub;
		int batteryNeeded = totalDistance * m_batteryConsumptionRate;
		
		// 20% safty margin
		return m_currentBattery < (batteryNeeded * 1.2f);
	}
	
	return false;
}

bool Robot::shouldVisitChargingStation(const Map& map) const
{
	float batteryPercent = static_cast<float>(m_currentBattery) / m_maxBattery;
	if (batteryPercent < 0.3f) return true;

	if (!m_packages.empty())
	{
		int distanceToDestination = std::abs(m_targetPosition.x - m_currentPosition.x) +
			std::abs(m_targetPosition.y - m_currentPosition.y);
		int distanceToHub = std::abs(map.getHubPosition().x - m_targetPosition.x) +
			std::abs(map.getHubPosition().y - m_targetPosition.y);
		int totalDistance = distanceToDestination + distanceToHub;
		int batteryNeeded = totalDistance * m_batteryConsumptionRate;
		return m_currentBattery < (batteryNeeded * 1.2f);
	}
	return false;
}

bool Scooter::shouldVisitChargingStation(const Map& map) const
{
	float batteryPercent = static_cast<float>(m_currentBattery) / m_maxBattery;
	if (batteryPercent < 0.35f) return true;

	if (!m_packages.empty())
	{
		int distanceToDestination = std::abs(m_targetPosition.x - m_currentPosition.x) +
			std::abs(m_targetPosition.y - m_currentPosition.y);
		int distanceToHub = std::abs(map.getHubPosition().x - m_targetPosition.x) +
			std::abs(map.getHubPosition().y - m_targetPosition.y);
		int totalDistance = distanceToDestination + distanceToHub;
		int batteryNeeded = totalDistance * m_batteryConsumptionRate;
		return m_currentBattery < (batteryNeeded * 1.2f);
	}
	return false;
}

void Drone::routeToNearestChargingStation(const Map& map)
{
	mapPosition nearestStation = map.getHubPosition();
	int minDistance = std::abs(nearestStation.x - m_currentPosition.x) + 
					  std::abs(nearestStation.y - m_currentPosition.y);
	
	for (const auto& station : map.getStationPositions())
	{
		int distance = std::abs(station.x - m_currentPosition.x) + 
					   std::abs(station.y - m_currentPosition.y);
		if (distance < minDistance)
		{
			minDistance = distance;
			nearestStation = station;
		}
	}
	
	m_chargingStationTarget = nearestStation;
	m_needsCharging = true;
	
	#ifdef DEBUG
	std::cout << "  [CHARGING ROUTE] Drone " << m_id << " routing to charging station at (" 
		<< nearestStation.x << ", " << nearestStation.y << ")\n";
	#endif
}

void Robot::routeToNearestChargingStation(const Map& map)
{
	mapPosition nearestStation = map.getHubPosition();
	int minDistance = std::abs(nearestStation.x - m_currentPosition.x) + 
					  std::abs(nearestStation.y - m_currentPosition.y);
	
	for (const auto& station : map.getStationPositions())
	{
		int distance = std::abs(station.x - m_currentPosition.x) + 
					   std::abs(station.y - m_currentPosition.y);
		if (distance < minDistance)
		{
			minDistance = distance;
			nearestStation = station;
		}
	}
	
	m_chargingStationTarget = nearestStation;
	m_needsCharging = true;
	
	#ifdef DEBUG
	std::cout << "  [CHARGING ROUTE] Robot " << m_id << " routing to charging station at (" 
		<< nearestStation.x << ", " << nearestStation.y << ")\n";
	#endif
}

void Scooter::routeToNearestChargingStation(const Map& map)
{
	mapPosition nearestStation = map.getHubPosition();
	int minDistance = std::abs(nearestStation.x - m_currentPosition.x) + 
					  std::abs(nearestStation.y - m_currentPosition.y);
	
	for (const auto& station : map.getStationPositions())
	{
		int distance = std::abs(station.x - m_currentPosition.x) + 
					   std::abs(station.y - m_currentPosition.y);
		if (distance < minDistance)
		{
			minDistance = distance;
			nearestStation = station;
		}
	}
	
	m_chargingStationTarget = nearestStation;
	m_needsCharging = true;
	
	#ifdef DEBUG
	std::cout << "  [CHARGING ROUTE] Scooter " << m_id << " routing to charging station at (" 
		<< nearestStation.x << ", " << nearestStation.y << ")\n";
	#endif
}

// deliver package functions ------------------------------

bool Drone::deliverPackage()
{
	if (m_packages.empty())
		return false;

	for (auto it = m_packages.begin(); it != m_packages.end(); ++it)
	{
		if ((*it)->getDestination().x == m_currentPosition.x && 
			(*it)->getDestination().y == m_currentPosition.y)
		{
			int reward = (*it)->getReward();
			bool wasLate = (*it)->getIsLate();
			
			// Apply late penalty
			if (wasLate)
			{
				reward -= PACKAGE_LATE_PENALTY;
			}
			
			m_personalRewards += reward;
			m_packages.erase(it);
			m_currentLoad--;
			
			#ifdef DEBUG
			std::cout << "  [DELIVERY] Drone " << m_id << " delivered package at (" 
				<< m_currentPosition.x << ", " << m_currentPosition.y << ") - Reward: $" << reward
				<< (wasLate ? " [LATE]" : "") << "\n";
			#endif
			
			return true;
		}
	}
	return false;
}

bool Robot::deliverPackage()
{
	if (m_packages.empty())
		return false;

	for (auto it = m_packages.begin(); it != m_packages.end(); ++it)
	{
		if ((*it)->getDestination().x == m_currentPosition.x &&
			(*it)->getDestination().y == m_currentPosition.y)
		{
			int reward = (*it)->getReward();
			bool wasLate = (*it)->getIsLate();

			if (wasLate)
			{
				reward -= PACKAGE_LATE_PENALTY;
			}

			m_personalRewards += reward;
			m_packages.erase(it);
			m_currentLoad--;

#ifdef DEBUG
			std::cout << "  [DELIVERY] Robot " << m_id << " delivered package at ("
				<< m_currentPosition.x << ", " << m_currentPosition.y << ") - Reward: $" << reward
				<< (wasLate ? " [LATE]" : "") << "\n";
#endif

			return true;
		}
	}
	return false;
}

bool Scooter::deliverPackage()
{
	if (m_packages.empty())
		return false;

	for (auto it = m_packages.begin(); it != m_packages.end(); ++it)
	{
		if ((*it)->getDestination().x == m_currentPosition.x &&
			(*it)->getDestination().y == m_currentPosition.y)
		{
			int reward = (*it)->getReward();
			bool wasLate = (*it)->getIsLate();

			if (wasLate)
			{
				reward -= PACKAGE_LATE_PENALTY;
			}

			m_personalRewards += reward;
			m_packages.erase(it);
			m_currentLoad--;

#ifdef DEBUG
			std::cout << "  [DELIVERY] Scooter " << m_id << " delivered package at ("
				<< m_currentPosition.x << ", " << m_currentPosition.y << ") - Reward: $" << reward
				<< (wasLate ? " [LATE]" : "") << "\n";
#endif

			return true;
		}
	}
	return false;
}

// assign package functions -------------------------------

bool Drone::assignPackage(std::unique_ptr<Package> package)
{
	if (m_currentLoad >= m_maxCapacity)
	{
		#ifdef DEBUG
		std::cout << "  [ASSIGN FAIL] Drone " << m_id << " at capacity (" 
			<< m_currentLoad << "/" << m_maxCapacity << ")\n";
		#endif
		return false;
	}

	mapPosition dest = package->getDestination();
	
	#ifdef DEBUG
	std::cout << "  [ASSIGN] Drone " << m_id << " assigned package to (" 
		<< dest.x << ", " << dest.y << ") - Reward: $" << package->getReward() << "\n";
	#endif

	m_targetPosition = dest;
	m_packages.push_back(std::move(package));
	m_currentLoad++;
	pathfindToTarget();
	
	#ifdef DEBUG
	std::cout << "    Path calculated: " << m_nextMoves.size() << " moves queued\n";
	#endif
	
	return true;
}

bool Robot::assignPackage(std::unique_ptr<Package> package, const Map& map)
{
	if (m_currentLoad >= m_maxCapacity)
	{
		#ifdef DEBUG
		std::cout << "  [ASSIGN FAIL] Robot " << m_id << " at capacity (" 
			<< m_currentLoad << "/" << m_maxCapacity << ")\n";
		#endif
		return false;
	}

	mapPosition dest = package->getDestination();
	
	#ifdef DEBUG
	std::cout << "  [ASSIGN] Robot " << m_id << " assigned package to (" 
		<< dest.x << ", " << dest.y << ") - Reward: $" << package->getReward() << "\n";
	#endif

	m_targetPosition = dest;
	m_packages.push_back(std::move(package));
	m_currentLoad++;
	pathfindToTarget(map);
	
	#ifdef DEBUG
	std::cout << "    Path calculated: " << m_nextMoves.size() << " moves queued\n";
	#endif
	
	return true;
}

bool Scooter::assignPackage(std::unique_ptr<Package> package, const Map& map)
{
	if (m_currentLoad >= m_maxCapacity)
	{
		#ifdef DEBUG
		std::cout << "  [ASSIGN FAIL] Scooter " << m_id << " at capacity (" 
			<< m_currentLoad << "/" << m_maxCapacity << ")\n";
		#endif
		return false;
	}

	mapPosition dest = package->getDestination();
	
	#ifdef DEBUG
	std::cout << "  [ASSIGN] Scooter " << m_id << " assigned package to (" 
		<< dest.x << ", " << dest.y << ") - Reward: $" << package->getReward() << "\n";
	#endif

	m_targetPosition = dest;
	m_packages.push_back(std::move(package));
	m_currentLoad++;
	pathfindToTarget(map);
	
	#ifdef DEBUG
	std::cout << "    Path calculated: " << m_nextMoves.size() << " moves queued\n";
	#endif
	
	return true;
}

// state handling functions -------------------------------

void Drone::handleState(const Map& map)
{
	switch (m_state)
	{
	// IDLE STATE -----------------------------------------
	case AgentState::IDLE:
		//check if dead
		if (m_currentBattery <= 0)
		{
			m_state = AgentState::DEAD;
			break;
		}

		//check if agent has packages and try to deliver
		if (m_currentLoad > 0)
		{
			// try to deliver
			if (deliverPackage())
			{
				// if delivered, check if more packages
				if (m_currentLoad == 0)
				{
					//return to hub
					pathfindToHub(map);
					if (!m_nextMoves.empty())
						m_state = AgentState::MOVING;
				}
			}
		}

		//check if the agent has queued moves
		if (!m_nextMoves.empty())
		{
			m_state = AgentState::MOVING;
		}

		break;
	// MOVING STATE ---------------------------------------
	case AgentState::MOVING:
		//check if dead
		if (m_currentBattery <= 0)
		{
			m_state = AgentState::DEAD;
			break;
		}

		// check battery
		if (shouldVisitChargingStation(map) && !m_needsCharging)
		{
			routeToNearestChargingStation(map);
			// reroute
			m_nextMoves.clear();
			m_targetPosition = m_chargingStationTarget;
			pathfindToTarget();
		}

		//check if agent is on charging station and needs charge
		if (m_currentBattery < m_maxBattery)
		{
			// check if on hub
			if (m_currentPosition.x == map.getHubPosition().x && 
				m_currentPosition.y == map.getHubPosition().y)
			{
				m_state = AgentState::CHARGING;
				m_needsCharging = false;
				break;
			}
			
			// check the rest of the stations
			for (auto stationPos : map.getStationPositions())
			{
				if (stationPos.x == m_currentPosition.x && stationPos.y == m_currentPosition.y)
				{
					m_state = AgentState::CHARGING;
					m_needsCharging = false;
					break;
				}
			}
		}

		//move based on speed 
		if (m_state == AgentState::MOVING && !m_nextMoves.empty())
		{
			for (int i = 0; i < m_speed && !m_nextMoves.empty(); ++i)
			{
				move();
			}
			
			//consume battery and add operation cost
			m_currentBattery -= m_batteryConsumptionRate;
			m_operationCostTotal += m_operationCost;
			
			// check if dead
			if (m_currentBattery <= 0)
			{
				m_currentBattery = 0;
				m_state = AgentState::DEAD;
				
				#ifdef DEBUG
				std::cout << "  [DEATH] Drone " << m_id << " ran out of battery at (" 
					<< m_currentPosition.x << ", " << m_currentPosition.y << ")\n";
				#endif
				break;
			}
			
			// check if reached destination
			if (m_nextMoves.empty())
			{
				if (m_needsCharging)
				{
					m_needsCharging = false;
					// reroute to original target if packages exist
					if (!m_packages.empty() && m_packages[0]->getDestination().x != m_currentPosition.x)
					{
						m_targetPosition = m_packages[0]->getDestination();
						pathfindToTarget();
					}
				}
				
				// deliver immediately
				if (m_currentLoad > 0 && deliverPackage())
				{
					// if delivered, check if more packages
					if (m_currentLoad == 0)
					{
						pathfindToHub(map);
					}
				}
				#ifdef DEBUG
				else if (m_currentLoad > 0)
				{
					std::cout << "  [DELIVERY FAIL] Drone " << m_id << " at (" << m_currentPosition.x << ", " << m_currentPosition.y 
						<< ") has package for (" << m_packages[0]->getDestination().x << ", " << m_packages[0]->getDestination().y << ")\n";
				}
				#endif
				
				// check moves
				if (m_nextMoves.empty())
				{
					m_state = AgentState::IDLE;
				}
			}
		}
		break;
	// CHARGING STATE -------------------------------------
	case AgentState::CHARGING:
		if (m_currentBattery >= m_maxBattery)
		{
			m_currentBattery = m_maxBattery;
			
			#ifdef DEBUG
			std::cout << "  [CHARGED] Drone " << m_id << " fully charged\n";
			#endif
			
			// check moves
			if (!m_packages.empty())
			{
				m_targetPosition = m_packages[0]->getDestination();
				pathfindToTarget();
				if (!m_nextMoves.empty())
					m_state = AgentState::MOVING;
				else
					m_state = AgentState::IDLE;
			}
			else if (!m_nextMoves.empty())
			{
				m_state = AgentState::MOVING;
			}
			else
			{
				m_state = AgentState::IDLE;
			}
		}
		else
		{
			chargeBattery();
		}
		break;
	// DEAD STATE -----------------------------------------
	case AgentState::DEAD:
	default:
		break;
	}
	
	m_ticksMoving++;
}

void Robot::handleState(const Map& map)
{
	switch (m_state)
	{
	case AgentState::IDLE:
		if (m_currentBattery <= 0)
		{
			m_state = AgentState::DEAD;
			break;
		}

		if (m_currentLoad > 0)
		{
			if (deliverPackage())
			{
				if (m_currentLoad == 0)
				{
					pathfindToHub(map);
					if (!m_nextMoves.empty())
						m_state = AgentState::MOVING;
				}
			}
		}

		if (!m_nextMoves.empty())
		{
			m_state = AgentState::MOVING;
		}
		break;

	case AgentState::MOVING:
		if (m_currentBattery <= 0)
		{
			m_state = AgentState::DEAD;
			break;
		}

		if (shouldVisitChargingStation(map) && !m_needsCharging)
		{
			routeToNearestChargingStation(map);
			m_nextMoves.clear();
			m_targetPosition = m_chargingStationTarget;
			pathfindToTarget(map);
		}

		if (m_currentBattery < m_maxBattery)
		{
			if (m_currentPosition.x == map.getHubPosition().x && 
				m_currentPosition.y == map.getHubPosition().y)
			{
				m_state = AgentState::CHARGING;
				m_needsCharging = false;
				break;
			}
			
			for (auto stationPos : map.getStationPositions())
			{
				if (stationPos.x == m_currentPosition.x && stationPos.y == m_currentPosition.y)
				{
					m_state = AgentState::CHARGING;
					m_needsCharging = false;
					break;
				}
			}
		}

		if (m_state == AgentState::MOVING && !m_nextMoves.empty())
		{
			for (int i = 0; i < m_speed && !m_nextMoves.empty(); ++i)
			{
				move();
			}
			
			m_currentBattery -= m_batteryConsumptionRate;
			m_operationCostTotal += m_operationCost;
			
			if (m_currentBattery <= 0)
			{
				m_currentBattery = 0;
				m_state = AgentState::DEAD;
				
				#ifdef DEBUG
				std::cout << "  [DEATH] Robot " << m_id << " ran out of battery at (" 
					<< m_currentPosition.x << ", " << m_currentPosition.y << ")\n";
				#endif
				break;
			}
			
			if (m_nextMoves.empty())
			{
				if (m_currentLoad > 0 && deliverPackage())
				{
					if (m_currentLoad == 0)
					{
						pathfindToHub(map);
					}
				}
				#ifdef DEBUG
				else if (m_currentLoad > 0)
				{
					std::cout << "  [DELIVERY FAIL] Robot " << m_id << " at (" << m_currentPosition.x << ", " << m_currentPosition.y 
						<< ") has package for (" << m_packages[0]->getDestination().x << ", " << m_packages[0]->getDestination().y << ")\n";
				}
				#endif
				
				if (m_nextMoves.empty())
				{
					m_state = AgentState::IDLE;
				}
			}
		}
		break;

	case AgentState::CHARGING:
		if (m_currentBattery >= m_maxBattery)
		{
			m_currentBattery = m_maxBattery;
			
			#ifdef DEBUG
			std::cout << "  [CHARGED] Robot " << m_id << " fully charged\n";
			#endif
			
			if (!m_packages.empty())
			{
				m_targetPosition = m_packages[0]->getDestination();
				pathfindToTarget(map);
				if (!m_nextMoves.empty())
					m_state = AgentState::MOVING;
				else
					m_state = AgentState::IDLE;
			}
			else if (!m_nextMoves.empty())
			{
				m_state = AgentState::MOVING;
			}
			else
			{
				m_state = AgentState::IDLE;
			}
		}
		else
		{
			chargeBattery();
		}
		break;

	case AgentState::DEAD:
	default:
		break;
	}
	
	m_ticksMoving++;
}

void Scooter::handleState(const Map& map)
{
	switch (m_state)
	{
	case AgentState::IDLE:
		if (m_currentBattery <= 0)
		{
			m_state = AgentState::DEAD;
			break;
		}

		if (m_currentLoad > 0)
		{
			if (deliverPackage())
			{
				if (m_currentLoad == 0)
				{
					pathfindToHub(map);
					if (!m_nextMoves.empty())
						m_state = AgentState::MOVING;
				}
			}
		}

		if (!m_nextMoves.empty())
		{
			m_state = AgentState::MOVING;
		}
		break;

	case AgentState::MOVING:
		if (m_currentBattery <= 0)
		{
			m_state = AgentState::DEAD;
			break;
		}

		if (shouldVisitChargingStation(map) && !m_needsCharging)
		{
			routeToNearestChargingStation(map);
			m_nextMoves.clear();
			m_targetPosition = m_chargingStationTarget;
			pathfindToTarget(map);
		}

		if (m_currentBattery < m_maxBattery)
		{
			if (m_currentPosition.x == map.getHubPosition().x && 
				m_currentPosition.y == map.getHubPosition().y)
			{
				m_state = AgentState::CHARGING;
				m_needsCharging = false;
				break;
			}
			
			for (auto stationPos : map.getStationPositions())
			{
				if (stationPos.x == m_currentPosition.x && stationPos.y == m_currentPosition.y)
				{
					m_state = AgentState::CHARGING;
					m_needsCharging = false;
					break;
				}
			}
		}

		if (m_state == AgentState::MOVING && !m_nextMoves.empty())
		{
			for (int i = 0; i < m_speed && !m_nextMoves.empty(); ++i)
			{
				move();
			}
			
			m_currentBattery -= m_batteryConsumptionRate;
			m_operationCostTotal += m_operationCost;
			
			if (m_currentBattery <= 0)
			{
				m_currentBattery = 0;
				m_state = AgentState::DEAD;
				
				#ifdef DEBUG
				std::cout << "  [DEATH] Scooter " << m_id << " ran out of battery at (" 
					<< m_currentPosition.x << ", " << m_currentPosition.y << ")\n";
				#endif
				break;
			}
			
			if (m_nextMoves.empty())
			{
				if (m_currentLoad > 0 && deliverPackage())
				{
					if (m_currentLoad == 0)
					{
						pathfindToHub(map);
					}
				}
				#ifdef DEBUG
				else if (m_currentLoad > 0)
				{
					std::cout << "  [DELIVERY FAIL] Scooter " << m_id << " at (" << m_currentPosition.x << ", " << m_currentPosition.y 
						<< ") has package for (" << m_packages[0]->getDestination().x << ", " << m_packages[0]->getDestination().y << ")\n";
				}
				#endif
				
				if (m_nextMoves.empty())
				{
					m_state = AgentState::IDLE;
				}
			}
		}
		break;

	case AgentState::CHARGING:
		if (m_currentBattery >= m_maxBattery)
		{
			m_currentBattery = m_maxBattery;
			
			#ifdef DEBUG
			std::cout << "  [CHARGED] Scooter " << m_id << " fully charged\n";
			#endif
			
			if (!m_packages.empty())
			{
				m_targetPosition = m_packages[0]->getDestination();
				pathfindToTarget(map);
				if (!m_nextMoves.empty())
					m_state = AgentState::MOVING;
				else
					m_state = AgentState::IDLE;
			}
			else if (!m_nextMoves.empty())
			{
				m_state = AgentState::MOVING;
			}
			else
			{
				m_state = AgentState::IDLE;
			}
		}
		else
		{
			chargeBattery();
		}
		break;

	case AgentState::DEAD:
	default:
		break;
	}
	
	m_ticksMoving++;
}