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

// --------------------------

mapPosition Robot::getCurrentPosition() const
{
	return m_currentPosition;
}

// --------------------------

mapPosition Scooter::getCurrentPosition() const
{
	return m_currentPosition;
}

// constructors -------------------------------------------

Drone::Drone(mapPosition startPos)
{
	m_id = s_nextID++;
	m_symbol = AgentSymbol::DRONE;
	m_state = AgentState::IDLE;
	m_ticksAlive = 0;

	m_targetPosition = m_currentPosition = startPos;
	m_speed = DRONE_SPEED;

	m_currentBattery = m_maxBattery = DRONE_MAX_BATTERY;
	m_batteryConsumptionRate = DRONE_BATTERY_CONSUMPTION_RATE;

	m_operationCost = DRONE_OPERATION_COST;
	m_operationCostTotal = 0;

	m_maxCapacity = DRONE_MAX_CAPACITY;
	m_currentLoad = 0;
}

// --------------------------

Robot::Robot(mapPosition startPos)
{
	m_id = s_nextID++;
	m_symbol = AgentSymbol::ROBOT;
	m_state = AgentState::IDLE;
	m_ticksAlive = 0;

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
	m_ticksAlive = 0;

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
	// TO DO implement move
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
	return false;
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

		//check if agent is on client position
		for (auto clientPos : map.getClientPositions())
		{
			if (clientPos.x == getCurrentPosition().x && clientPos.y == getCurrentPosition().y)
			{
				if (deliverPackage())
				{
					pathfindToHub(map);
					break;
				}
				else
				{
					throw std::runtime_error("Drone tried to deliver package but failed");
				}
			}
		}

		//check if the agent has queued moves
		if (!m_nextMoves.empty())
		{
			m_state = AgentState::MOVING;
			for (int i = 0; i < m_speed; ++i)
			{
				move();
			}
			break;
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

		//check if agent is on charging station
		for (auto stationPos : map.getStationPositions())
		{
			if (stationPos.x == getCurrentPosition().x && stationPos.y == getCurrentPosition().y)
			{
				m_state = AgentState::CHARGING;
				break;
			}
		}

		//check if the agent has queued moves
		if (!m_nextMoves.empty())
		{
			move();
		}
		else
		{
			m_state = AgentState::IDLE;
		}
		break;
	// CHARGING STATE -------------------------------------
	case AgentState::CHARGING:
		if (m_currentBattery == m_maxBattery)
		{
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