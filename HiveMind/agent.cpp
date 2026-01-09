#include "agent.h"
#include "constants.h"
#include "map.h"
#include <stdexcept>

// static members -----------------------------------------
int Drone::s_nextID = 1;
int Robot::s_nextID = 1;
int Scooter::s_nextID = 1;

// ID getters ---------------------------------------------

int Drone::getID() const
{
	return m_id;
}

// --------------------------

int Robot::getID() const
{
	return m_id;
}

// --------------------------

int Scooter::getID() const
{
	return m_id;
}

// state getters ------------------------------------------

AgentState Drone::getState() const
{
	return m_state;
}

// --------------------------

AgentState Robot::getState() const
{
	return m_state;
}

// --------------------------

AgentState Scooter::getState() const
{
	return m_state;
}

// symbol getters -----------------------------------------

AgentSymbol Drone::getSymbol() const
{
	return m_symbol;
}

// --------------------------

AgentSymbol Robot::getSymbol() const
{
	return m_symbol;
}

// --------------------------

AgentSymbol Scooter::getSymbol() const
{
	return m_symbol;
}

// position getters ---------------------------------------

mapPosition Drone::getCurrentPosition() const
{
	return m_currentPosition;
}

std::vector<std::unique_ptr<Package>>& Drone::getPackages()
{
	return m_packages;
}

// --------------------------

mapPosition Robot::getCurrentPosition() const
{
	return m_currentPosition;
}

std::vector<std::unique_ptr<Package>>& Robot::getPackages()
{
	return m_packages;
}

// --------------------------

mapPosition Scooter::getCurrentPosition() const
{
	return m_currentPosition;
}

std::vector<std::unique_ptr<Package>>& Scooter::getPackages()
{
	return m_packages;
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
	// TO DO implement move
}

// --------------------------

void Scooter::move()
{
	// TO DO implement move
}

// battery charging functions -----------------------------

void Drone::chargeBattery()
{
	if (m_currentBattery < m_maxBattery)
		m_currentBattery += static_cast<int>(m_maxBattery * BATTERY_RECHARGE_RATE);
	else
		m_currentBattery = m_maxBattery;
}

// --------------------------

void Robot::chargeBattery()
{
	if (m_currentBattery < m_maxBattery)
		m_currentBattery += static_cast<int>(m_maxBattery * BATTERY_RECHARGE_RATE);
	else
		m_currentBattery = m_maxBattery;
}

// --------------------------

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

// pathfinding to hub -------------------------------------
	
void Drone::pathfindToHub(const Map& map)
{
	m_targetPosition = map.getHubPosition();
	pathfindToTarget();
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
			m_personalRewards += (*it)->getReward();
			m_packages.erase(it);
			m_currentLoad--;
			return true;
		}
	}
	return false;
}

// assign package functions -------------------------------

bool Drone::assignPackage(std::unique_ptr<Package> package)
{
	if (m_currentLoad >= m_maxCapacity)
		return false;

	m_targetPosition = package->getDestination();
	m_packages.push_back(std::move(package));
	m_currentLoad++;
	pathfindToTarget();
	return true;
}

// operation cost getters ---------------------------------

int Drone::getTotalOperationCost() const
{
	return m_operationCostTotal;
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

		//check if agent is on client position and has packages
		if (m_currentLoad > 0)
		{
			for (auto clientPos : map.getClientPositions())
			{
				if (clientPos.x == m_currentPosition.x && clientPos.y == m_currentPosition.y)
				{
					deliverPackage();
					
					if (m_currentLoad == 0)
					{
						//return to hub
						pathfindToHub(map);
						if (!m_nextMoves.empty())
							m_state = AgentState::MOVING;
					}
					break;
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

		//check if agent is on charging station and needs charge
		if (m_currentBattery < m_maxBattery)
		{
			for (auto stationPos : map.getStationPositions())
			{
				if (stationPos.x == m_currentPosition.x && stationPos.y == m_currentPosition.y)
				{
					m_state = AgentState::CHARGING;
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
				if (m_state == AgentState::DEAD)
					break;
			}
			
			// check if reached destination
			if (m_nextMoves.empty())
			{
				m_state = AgentState::IDLE;
			}
		}
		break;
	// CHARGING STATE -------------------------------------
	case AgentState::CHARGING:
		if (m_currentBattery >= m_maxBattery)
		{
			m_currentBattery = m_maxBattery;
			if (!m_nextMoves.empty())
				m_state = AgentState::MOVING;
			else
				m_state = AgentState::IDLE;
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

// --------------------------

void Robot::handleState()
{
	// TO DO implement handle state
}

// --------------------------

void Scooter::handleState()
{
	// TO DO implement handle state
}