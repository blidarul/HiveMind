#include "hivemind.h"
#include "constants.h"
#include <cmath>
#include <algorithm>
#include <limits>

#ifdef DEBUG
#include <iostream>
#endif

HiveMind::HiveMind(const Map& map)
    : m_map(map)
{
}

int HiveMind::calculateDistance(mapPosition from, mapPosition to) const
{
    // estimate
    return std::abs(to.x - from.x) + std::abs(to.y - from.y);
}

mapPosition HiveMind::findNearestChargingStation(mapPosition from) const
{
    mapPosition nearestStation = m_map.getHubPosition();
    int minDistance = calculateDistance(from, nearestStation);
    
    for (const auto& station : m_map.getStationPositions())
    {
        int distance = calculateDistance(from, station);
        if (distance < minDistance)
        {
            minDistance = distance;
            nearestStation = station;
        }
    }
    
    return nearestStation;
}

int HiveMind::calculateBatteryNeeded(IAgent* agent, mapPosition from, mapPosition to) const
{
    int distance = calculateDistance(from, to);
    
    int batteryConsumption = 0;
    AgentSymbol symbol = agent->getSymbol();
    
    if (symbol == AgentSymbol::DRONE)
        batteryConsumption = DRONE_BATTERY_CONSUMPTION_RATE;
    else if (symbol == AgentSymbol::ROBOT)
        batteryConsumption = ROBOT_BATTERY_CONSUMPTION_RATE;
    else if (symbol == AgentSymbol::SCOOTER)
        batteryConsumption = SCOOTER_BATTERY_CONSUMPTION_RATE;
    
    return distance * batteryConsumption;
}

bool HiveMind::needsCharging(IAgent* agent, const Package& package) const
{
    mapPosition agentPos = agent->getCurrentPosition();
    mapPosition packageDest = package.getDestination();
    mapPosition hubPos = m_map.getHubPosition();
    
    int batteryToDestination = calculateBatteryNeeded(agent, agentPos, packageDest);
    int batteryToHub = calculateBatteryNeeded(agent, packageDest, hubPos);
    int totalBatteryNeeded = batteryToDestination + batteryToHub;
    
    int maxBattery = 0;
    AgentSymbol symbol = agent->getSymbol();
    if (symbol == AgentSymbol::DRONE)
        maxBattery = DRONE_MAX_BATTERY;
    else if (symbol == AgentSymbol::ROBOT)
        maxBattery = ROBOT_MAX_BATTERY;
    else if (symbol == AgentSymbol::SCOOTER)
        maxBattery = SCOOTER_MAX_BATTERY;
    
    return totalBatteryNeeded > (maxBattery * 0.7f);
}

int HiveMind::calculateTripCost(IAgent* agent, const Package& package) const
{
    mapPosition agentPos = agent->getCurrentPosition();
    mapPosition packageDest = package.getDestination();
    mapPosition hubPos = m_map.getHubPosition();
    
    int totalDistance = 0;
    
    if (needsCharging(agent, package))
    {
        mapPosition chargingStation = findNearestChargingStation(agentPos);
        
        totalDistance += calculateDistance(agentPos, chargingStation);
        totalDistance += calculateDistance(chargingStation, packageDest);
        totalDistance += calculateDistance(packageDest, hubPos);
        
        #ifdef DEBUG
        std::cout << "    [ROUTING] Agent " << agent->getID() 
            << " will use charging station at (" << chargingStation.x << ", " << chargingStation.y << ")\n";
        #endif
    }
    else
    {
        totalDistance += calculateDistance(agentPos, packageDest);
        totalDistance += calculateDistance(packageDest, hubPos);
    }
    
    int speed = 1;
    AgentSymbol symbol = agent->getSymbol();
    if (symbol == AgentSymbol::DRONE) speed = DRONE_SPEED;
    else if (symbol == AgentSymbol::ROBOT) speed = ROBOT_SPEED;
    else if (symbol == AgentSymbol::SCOOTER) speed = SCOOTER_SPEED;
    
    int ticksNeeded = (totalDistance + speed - 1) / speed;
    
    int operationCostPerTick = 0;
    if (symbol == AgentSymbol::DRONE) operationCostPerTick = DRONE_OPERATION_COST;
    else if (symbol == AgentSymbol::ROBOT) operationCostPerTick = ROBOT_OPERATION_COST;
    else if (symbol == AgentSymbol::SCOOTER) operationCostPerTick = SCOOTER_OPERATION_COST;
    
    if (needsCharging(agent, package))
    {
        ticksNeeded += 4;
    }
    
    return ticksNeeded * operationCostPerTick;
}

bool HiveMind::canCompleteDelivery(IAgent* agent, const Package& package) const
{
    mapPosition agentPos = agent->getCurrentPosition();
    mapPosition packageDest = package.getDestination();
    
    int currentBattery = agent->getCurrentBattery();
    int maxBattery = agent->getMaxBattery();
    
    if (currentBattery < maxBattery * 0.2f)
    {
        #ifdef DEBUG
        std::cout << "    [BATTERY CHECK] Agent battery too low (" << currentBattery << "/" << maxBattery << ")\n";
        #endif
        return false;
    }
    
    if (needsCharging(agent, package))
    {
        mapPosition chargingStation = findNearestChargingStation(agentPos);
        int batteryToStation = calculateBatteryNeeded(agent, agentPos, chargingStation);
        
        if (batteryToStation > currentBattery)
        {
            #ifdef DEBUG
            std::cout << "    [BATTERY CHECK] Not enough battery to reach charging station! (Need: " 
                << batteryToStation << ", Have: " << currentBattery << ")\n";
            #endif
            return false;
        }
        
        int batteryToDestination = calculateBatteryNeeded(agent, chargingStation, packageDest);
        
        if (batteryToDestination > maxBattery * 0.95f)
        {
            #ifdef DEBUG
            std::cout << "    [BATTERY CHECK] Too far from charging station to destination!\n";
            #endif
            return false;
        }
        
        mapPosition hubPos = m_map.getHubPosition();
        int batteryToHub = calculateBatteryNeeded(agent, packageDest, hubPos);
        
        if (batteryToHub > maxBattery * 0.95f)
        {
            mapPosition nearStation = findNearestChargingStation(packageDest);
            int batteryToNearStation = calculateBatteryNeeded(agent, packageDest, nearStation);
            
            if (batteryToNearStation > maxBattery * 0.95f)
            {
                #ifdef DEBUG
                std::cout << "    [BATTERY CHECK] Can't return from destination!\n";
                #endif
                return false;
            }
        }
        
        return true;
    }
    else
    {
        int batteryToDestination = calculateBatteryNeeded(agent, agentPos, packageDest);
        mapPosition hubPos = m_map.getHubPosition();
        int batteryToHub = calculateBatteryNeeded(agent, packageDest, hubPos);
        int totalBatteryNeeded = batteryToDestination + batteryToHub;
        
        if (totalBatteryNeeded > currentBattery)
        {
            #ifdef DEBUG
            std::cout << "    [BATTERY CHECK] Direct route needs " << totalBatteryNeeded 
                << " but agent has " << currentBattery << "\n";
            #endif
            return false;
        }
        
        return true;
    }
}

float HiveMind::calculateAgentScore(IAgent* agent, const Package& package, int distance) const
{
    if (agent->getState() != AgentState::IDLE)
        return std::numeric_limits<float>::max();
    
    if (!agent->getPackages().empty())
        return std::numeric_limits<float>::max();
    
    int tripCost = calculateTripCost(agent, package);
    int reward = package.getReward();

    float costRatio = static_cast<float>(tripCost) / static_cast<float>(reward);
    float distanceFactor = 1.0f + (static_cast<float>(distance) / 100.0f);
    
    int remainingTicks = package.getRemainingTicks();
    float urgencyFactor = 1.0f;
    if (remainingTicks < 10) {
        urgencyFactor = 0.5f;
    } else if (remainingTicks < 15) {
        urgencyFactor = 0.75f;
    }
    
    float chargingPenalty = needsCharging(agent, package) ? 1.2f : 1.0f;
    
    float batteryPercent = static_cast<float>(agent->getCurrentBattery()) / agent->getMaxBattery();
    float batteryFactor = 2.0f - batteryPercent;
    
    float speedBonus = 1.0f;
    if (remainingTicks < 10)
    {
        AgentSymbol symbol = agent->getSymbol();
        if (symbol == AgentSymbol::DRONE)
            speedBonus = 0.8f;
        else if (symbol == AgentSymbol::SCOOTER)
            speedBonus = 0.9f;
        else
            speedBonus = 1.1f;
    }
    
    return costRatio * distanceFactor * urgencyFactor * chargingPenalty * batteryFactor * speedBonus;
}

IAgent* HiveMind::selectBestAgent(
    const Package& package,
    std::vector<std::unique_ptr<IAgent>>& agents)
{
    IAgent* bestAgent = nullptr;
    float bestScore = std::numeric_limits<float>::max();
    
    mapPosition packageDest = package.getDestination();
    
    #ifdef DEBUG
    int idleCount = 0;
    int busyCount = 0;
    int deadCount = 0;
    #endif
    
    for (auto& agent : agents)
    {
        if (agent->getState() == AgentState::DEAD)
        {
            #ifdef DEBUG
            deadCount++;
            #endif
            continue;
        }
        
        if (agent->getState() != AgentState::IDLE)
        {
            #ifdef DEBUG
            busyCount++;
            #endif
            continue;
        }
        
        #ifdef DEBUG
        idleCount++;
        #endif
        
        if (!agent->getPackages().empty())
            continue;
        
        if (!canCompleteDelivery(agent.get(), package))
            continue;
        
        mapPosition agentPos = agent->getCurrentPosition();
        int distance = calculateDistance(agentPos, packageDest);
        
        float score = calculateAgentScore(agent.get(), package, distance);
        
        if (score < bestScore)
        {
            bestScore = score;
            bestAgent = agent.get();
        }
    }
    
    #ifdef DEBUG
    if (bestAgent == nullptr)
    {
        std::cout << "  [WARNING] No suitable agent found! (IDLE: " << idleCount 
            << ", BUSY: " << busyCount << ", DEAD: " << deadCount << ")\n";
    }
    #endif
    
    return bestAgent;
}

void HiveMind::assignPackages(
    std::vector<std::unique_ptr<Package>>& packagesInBase,
    std::vector<std::unique_ptr<IAgent>>& agents)
{
    if (packagesInBase.empty())
        return;
    
    #ifdef DEBUG
    std::cout << "  [HIVEMIND] Attempting to assign " << packagesInBase.size() << " package(s)...\n";
    #endif
    
    std::sort(packagesInBase.begin(), packagesInBase.end(),
        [](const std::unique_ptr<Package>& a, const std::unique_ptr<Package>& b) {
            if (a->getRemainingTicks() != b->getRemainingTicks())
                return a->getRemainingTicks() < b->getRemainingTicks();
            return a->getReward() > b->getReward();
        });
    
    int assignmentsThisRound = 0;
    int maxAssignmentsPerAgent = 1;
    
    for (auto it = packagesInBase.begin(); it != packagesInBase.end(); )
    {
        IAgent* bestAgent = selectBestAgent(**it, agents);
        
        if (bestAgent != nullptr)
        {
            #ifdef DEBUG
            std::cout << "    Best agent selected: " << static_cast<char>(bestAgent->getSymbol()) 
                << " ID " << bestAgent->getID() 
                << " (Battery: " << bestAgent->getCurrentBattery() << "/" << bestAgent->getMaxBattery() << ")\n";
            #endif
            
            bool assigned = false;
            
            if (bestAgent->getSymbol() == AgentSymbol::DRONE)
            {
                Drone* drone = static_cast<Drone*>(bestAgent);
                assigned = drone->assignPackage(std::move(*it));
            }
            else if (bestAgent->getSymbol() == AgentSymbol::ROBOT)
            {
                Robot* robot = static_cast<Robot*>(bestAgent);
                assigned = robot->assignPackage(std::move(*it), m_map);
            }
            else if (bestAgent->getSymbol() == AgentSymbol::SCOOTER)
            {
                Scooter* scooter = static_cast<Scooter*>(bestAgent);
                assigned = scooter->assignPackage(std::move(*it), m_map);
            }
            
            if (assigned)
            {
                assignmentsThisRound++;
                it = packagesInBase.erase(it);
                continue;
            }
        }
        
        ++it;
    }
    
    #ifdef DEBUG
    if (assignmentsThisRound > 0)
    {
        std::cout << "  [HIVEMIND] Successfully assigned " << assignmentsThisRound << " package(s)\n";
        std::cout << "  [HIVEMIND] Remaining in base: " << packagesInBase.size() << "\n";
    }
    #endif
}
