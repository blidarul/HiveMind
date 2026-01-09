#include "constants.h"
#include "map.h"
#include "package.h"
#include <random>

int Package::s_nextID = 1;

Package::Package(const Map& map)
	: m_id(s_nextID++)
	, isLate(false)
{
	m_destination = PackageGenerator::getInstance(map).generatePackageDestination(map);
	m_remainingTicks = PackageGenerator::getInstance(map).generatePackageTicks();
	m_reward = PackageGenerator::getInstance(map).generatePackageReward();
}

mapPosition Package::getDestination() const
{
	return m_destination;
}

int Package::getRemainingTicks() const
{
	return m_remainingTicks;
}

int Package::getReward() const
{
	return m_reward;
}

bool Package::getIsLate() const
{
	return isLate;
}

void Package::decrementTick()
{
	if (m_remainingTicks > 0)
	{
		m_remainingTicks--;
	}
	
	if (m_remainingTicks == 0 && !isLate)
	{
		isLate = true;
	}
}

PackageGenerator::PackageGenerator(const Map& map)
	: m_distTicks(MIN_PACKAGE_TICKS, MAX_PACKAGE_TICKS)
	, m_distReward(MIN_PACKAGE_REWARD, MAX_PACKAGE_REWARD)
{
	m_gen.seed(m_rd());
	m_distClient = std::uniform_int_distribution<>(0, static_cast<int>(map.getClientCount()) - 1);
}

PackageGenerator& PackageGenerator::getInstance(const Map& map)
{
	static PackageGenerator instance(map);
	return instance;
}

mapPosition PackageGenerator::generatePackageDestination(const Map& map)
{
	int clientIndex = m_distClient(m_gen);
	return map.getClientPositions()[clientIndex];
}

int PackageGenerator::generatePackageTicks()
{
	return m_distTicks(m_gen);
}

int PackageGenerator::generatePackageReward()
{
	return m_distReward(m_gen);
}