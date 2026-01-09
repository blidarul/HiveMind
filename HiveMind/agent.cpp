#include "agent.h"
#include "constants.h"

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
		m_currentBattery += m_maxBattery / 4;
	else
		m_currentBattery = m_maxBattery;
}

// --------------------------

void Robot::chargeBattery()
{
	if (m_currentBattery < m_maxBattery)
		m_currentBattery += m_maxBattery / 4;
	else
		m_currentBattery = m_maxBattery;
}

// --------------------------

void Scooter::chargeBattery()
{
	if (m_currentBattery < m_maxBattery)
		m_currentBattery += m_maxBattery / 4;
	else
		m_currentBattery = m_maxBattery;
}

// state handling functions -------------------------------

void Drone::handleState()
{
	// TO DO implement handle state
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