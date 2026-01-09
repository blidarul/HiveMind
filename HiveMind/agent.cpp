#include "agent.h"
#include "constants.h"

// Initialize static members
int Drone::s_nextID = 1;
int Robot::s_nextID = 1;
int Scooter::s_nextID = 1;

// IMPLEMENTATION OF DRONE CLASS

int Drone::getID() const
{
	return m_id;
}

AgentState Drone::getState() const
{
	return m_state;
}

AgentSymbol Drone::getSymbol() const
{
	return m_symbol;
}

mapPosition Drone::getCurrentPosition() const
{
	return m_currentPosition;
}

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

	m_maxCapacity = DRONE_MAX_CAPACITY;
	m_currentLoad = 0;
}

void Drone::moveTo(mapPosition position)
{
	m_targetPosition = position;
	m_state = AgentState::MOVING;
}

void Drone::chargeBattery()
{
	if (m_currentBattery < m_maxBattery)
		m_currentBattery += m_maxBattery / 4;
	else
		m_currentBattery = m_maxBattery;
}

// IMPLEMENTATION OF ROBOT CLASS

int Robot::getID() const
{
	return m_id;
}

AgentState Robot::getState() const
{
	return m_state;
}

AgentSymbol Robot::getSymbol() const
{
	return m_symbol;
}

mapPosition Robot::getCurrentPosition() const
{
	return m_currentPosition;
}

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

	m_maxCapacity = ROBOT_MAX_CAPACITY;
	m_currentLoad = 0;
}

void Robot::moveTo(mapPosition position)
{
	m_targetPosition = position;
	m_state = AgentState::MOVING;
}

void Robot::chargeBattery()
{
	if (m_currentBattery < m_maxBattery)
		m_currentBattery += m_maxBattery / 4;
	else
		m_currentBattery = m_maxBattery;
}

// IMPLEMENTATION OF SCOOTER CLASS

int Scooter::getID() const
{
	return m_id;
}

AgentState Scooter::getState() const
{
	return m_state;
}

AgentSymbol Scooter::getSymbol() const
{
	return m_symbol;
}

mapPosition Scooter::getCurrentPosition() const
{
	return m_currentPosition;
}

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

	m_maxCapacity = SCOOTER_MAX_CAPACITY;
	m_currentLoad = 0;
}

void Scooter::moveTo(mapPosition position)
{
	m_targetPosition = position;
	m_state = AgentState::MOVING;
}

void Scooter::chargeBattery()
{
	if (m_currentBattery < m_maxBattery)
		m_currentBattery += m_maxBattery / 4;
	else
		m_currentBattery = m_maxBattery;
}