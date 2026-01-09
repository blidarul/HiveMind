#pragma once
#include <random>
#include "map.h"

class Package
{
public:
	Package(const Map& map, mapPosition destination, int ticks, int reward);
	mapPosition getDestination() const;
	int getRemainingTicks() const;
	int getReward() const;

private:
	static int s_nextID;
	int m_id;
	int m_remainingTicks;
	int m_reward;

	bool isLate;

	mapPosition m_destination;
};

// singleton class
class PackageGenerator
{
public:
	static PackageGenerator& getInstance(const Map& map);
	
	PackageGenerator(const PackageGenerator&) = delete;
	PackageGenerator& operator=(const PackageGenerator&) = delete;
	
	mapPosition generatePackageDestination(const Map& map);
	int generatePackageTicks();
	int generatePackageReward();

private:
	PackageGenerator(const Map& map);
	
	std::random_device m_rd;
	std::mt19937 m_gen;

	std::uniform_int_distribution<> m_distClient;
	std::uniform_int_distribution<> m_distTicks;
	std::uniform_int_distribution<> m_distReward;
};