#include "hivemind.h"
#include "constants.h"
#include <cmath>
#include <algorithm>
#include <limits>

#ifdef DEBUG
#include <iostream>
#endif

// Strategy Pattern Implementation: HiveMind algorithm
HiveMind::HiveMind(const Map& map)
    : m_map(map)
{
}

// Calculate Manhattan distance (for drones) or use floodfill distance (for ground units)
int HiveMind::calculateDistance(mapPosition from, mapPosition to) const
{
    // Manhattan distance as base estimate
    return std::abs(to.x - from.x) + std::abs(to.y - from.y);
}

// Find nearest charging station from given position
mapPosition HiveMind::findNearestChargingStation(mapPosition from) const
{
    mapPosition nearestStation = m_map.getHubPosition(); // Hub is always a charging station
    int minDistance = calculateDistance(from, nearestStation);
    
    // Check all stations
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

// Calculate battery needed for a trip segment
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

// Check if agent needs charging station routing
bool HiveMind::needsCharging(IAgent* agent, const Package& package) const
{
    mapPosition agentPos = agent->getCurrentPosition();
    mapPosition packageDest = package.getDestination();
    mapPosition hubPos = m_map.getHubPosition();
    
    // Calculate battery needed for direct route
    int batteryToDestination = calculateBatteryNeeded(agent, agentPos, packageDest);
    int batteryToHub = calculateBatteryNeeded(agent, packageDest, hubPos);
    int totalBatteryNeeded = batteryToDestination + batteryToHub;
    
    // Get agent's max battery
    int maxBattery = 0;
    AgentSymbol symbol = agent->getSymbol();
    if (symbol == AgentSymbol::DRONE)
        maxBattery = DRONE_MAX_BATTERY;
    else if (symbol == AgentSymbol::ROBOT)
        maxBattery = ROBOT_MAX_BATTERY;
    else if (symbol == AgentSymbol::SCOOTER)
        maxBattery = SCOOTER_MAX_BATTERY;
    
    // Need charging if direct route uses more than 70% of max battery
    return totalBatteryNeeded > (maxBattery * 0.7f);
}

// Calculate total operational cost for this delivery
int HiveMind::calculateTripCost(IAgent* agent, const Package& package) const
{
    mapPosition agentPos = agent->getCurrentPosition();
    mapPosition packageDest = package.getDestination();
    mapPosition hubPos = m_map.getHubPosition();
    
    int totalDistance = 0;
    
    // Check if routing through charging station is needed
    if (needsCharging(agent, package))
    {
        // Route: Agent -> Charging Station -> Destination -> Hub
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
        // Direct route: Agent -> Destination -> Hub
        totalDistance += calculateDistance(agentPos, packageDest);
        totalDistance += calculateDistance(packageDest, hubPos);
    }
    
    // Estimate ticks needed (distance / speed)
    int speed = 1;
    AgentSymbol symbol = agent->getSymbol();
    if (symbol == AgentSymbol::DRONE) speed = DRONE_SPEED;
    else if (symbol == AgentSymbol::ROBOT) speed = ROBOT_SPEED;
    else if (symbol == AgentSymbol::SCOOTER) speed = SCOOTER_SPEED;
    
    int ticksNeeded = (totalDistance + speed - 1) / speed; // Ceiling division
    
    // Calculate operational cost
    int operationCostPerTick = 0;
    if (symbol == AgentSymbol::DRONE) operationCostPerTick = DRONE_OPERATION_COST;
    else if (symbol == AgentSymbol::ROBOT) operationCostPerTick = ROBOT_OPERATION_COST;
    else if (symbol == AgentSymbol::SCOOTER) operationCostPerTick = SCOOTER_OPERATION_COST;
    
    // Add charging time cost (estimated 4 ticks to fully charge)
    if (needsCharging(agent, package))
    {
        ticksNeeded += 4; // Charging time
    }
    
    return ticksNeeded * operationCostPerTick;
}

// Check if agent has enough battery to complete the delivery (with charging)
bool HiveMind::canCompleteDelivery(IAgent* agent, const Package& package) const
{
    mapPosition agentPos = agent->getCurrentPosition();
    mapPosition packageDest = package.getDestination();
    
    // Get agent's ACTUAL current battery (not just max)
    int currentBattery = agent->getCurrentBattery();
    int maxBattery = agent->getMaxBattery();
    
    // If agent is critically low, reject assignment
    if (currentBattery < maxBattery * 0.2f)
    {
        #ifdef DEBUG
        std::cout << "    [BATTERY CHECK] Agent battery too low (" << currentBattery << "/" << maxBattery << ")\n";
        #endif
        return false;
    }
    
    // With charging station routing, validate each segment
    if (needsCharging(agent, package))
    {
        // Check: Agent -> Charging Station (using CURRENT battery)
        mapPosition chargingStation = findNearestChargingStation(agentPos);
        int batteryToStation = calculateBatteryNeeded(agent, agentPos, chargingStation);
        
        if (batteryToStation > currentBattery) // Use actual battery, not max
        {
            #ifdef DEBUG
            std::cout << "    [BATTERY CHECK] Not enough battery to reach charging station! (Need: " 
                << batteryToStation << ", Have: " << currentBattery << ")\n";
            #endif
            return false;
        }
        
        // Check: Charging Station -> Destination (after full charge)
        int batteryToDestination = calculateBatteryNeeded(agent, chargingStation, packageDest);
        
        if (batteryToDestination > maxBattery * 0.95f)
        {
            #ifdef DEBUG
            std::cout << "    [BATTERY CHECK] Too far from charging station to destination!\n";
            #endif
            return false;
        }
        
        // Check: Destination -> Hub (or another charging station)
        mapPosition hubPos = m_map.getHubPosition();
        int batteryToHub = calculateBatteryNeeded(agent, packageDest, hubPos);
        
        if (batteryToHub > maxBattery * 0.95f)
        {
            // Try finding a charging station near destination
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
        // Direct route - check if CURRENT battery is sufficient
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

// Score function: lower is better (represents cost-benefit ratio)
float HiveMind::calculateAgentScore(IAgent* agent, const Package& package, int distance) const
{
    // Can't assign to agents that aren't idle or are dead
    if (agent->getState() != AgentState::IDLE)
        return std::numeric_limits<float>::max();
    
    // Check if agent already has packages
    if (!agent->getPackages().empty())
        return std::numeric_limits<float>::max();
    
    // Calculate operational cost (includes charging station routing)
    int tripCost = calculateTripCost(agent, package);
    int reward = package.getReward();
    
    // Score = (Cost / Reward) * Distance factor * Urgency factor * Battery factor
    // Lower score is better
    float costRatio = static_cast<float>(tripCost) / static_cast<float>(reward);
    float distanceFactor = 1.0f + (static_cast<float>(distance) / 100.0f);
    
    // Consider urgency (remaining ticks)
    int remainingTicks = package.getRemainingTicks();
    float urgencyFactor = 1.0f;
    if (remainingTicks < 10) {
        urgencyFactor = 0.5f; // Prioritize urgent packages
    } else if (remainingTicks < 15) {
        urgencyFactor = 0.75f;
    }
    
    // Penalize if charging is needed (adds complexity and time)
    float chargingPenalty = needsCharging(agent, package) ? 1.2f : 1.0f;
    
    // NEW: Battery efficiency factor - prefer agents with fuller batteries
    float batteryPercent = static_cast<float>(agent->getCurrentBattery()) / agent->getMaxBattery();
    float batteryFactor = 2.0f - batteryPercent; // Range: 1.0 (full) to 2.0 (empty)
    // Agents with less battery get worse score (higher value)
    
    // NEW: Speed bonus - faster agents get better scores for urgent packages
    float speedBonus = 1.0f;
    if (remainingTicks < 10)
    {
        AgentSymbol symbol = agent->getSymbol();
        if (symbol == AgentSymbol::DRONE)
            speedBonus = 0.8f; // Prefer drones for urgent deliveries
        else if (symbol == AgentSymbol::SCOOTER)
            speedBonus = 0.9f;
        else // Robot
            speedBonus = 1.1f; // Penalize slow robots for urgent packages
    }
    
    return costRatio * distanceFactor * urgencyFactor * chargingPenalty * batteryFactor * speedBonus;
}

// Select the best agent for a given package
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
        // Skip dead agents
        if (agent->getState() == AgentState::DEAD)
        {
            #ifdef DEBUG
            deadCount++;
            #endif
            continue;
        }
        
        // Skip agents that are already busy
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
        
        // Skip agents that already have packages
        if (!agent->getPackages().empty())
            continue;
        
        // Check if agent can complete the delivery (with charging stations)
        if (!canCompleteDelivery(agent.get(), package))
            continue;
        
        // Calculate distance
        mapPosition agentPos = agent->getCurrentPosition();
        int distance = calculateDistance(agentPos, packageDest);
        
        // Calculate score
        float score = calculateAgentScore(agent.get(), package, distance);
        
        // Select agent with best (lowest) score
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

// Main algorithm: assign packages to agents
void HiveMind::assignPackages(
    std::vector<std::unique_ptr<Package>>& packagesInBase,
    std::vector<std::unique_ptr<IAgent>>& agents)
{
    if (packagesInBase.empty())
        return;
    
    #ifdef DEBUG
    std::cout << "  [HIVEMIND] Attempting to assign " << packagesInBase.size() << " package(s)...\n";
    #endif
    
    // NEW: Multi-criteria sorting for better optimization
    // Sort packages by: 1) Urgency (remaining ticks), 2) Reward (higher first)
    std::sort(packagesInBase.begin(), packagesInBase.end(),
        [](const std::unique_ptr<Package>& a, const std::unique_ptr<Package>& b) {
            // Primary: Urgency
            if (a->getRemainingTicks() != b->getRemainingTicks())
                return a->getRemainingTicks() < b->getRemainingTicks();
            // Secondary: Higher reward
            return a->getReward() > b->getReward();
        });
    
    // NEW: Track assignment statistics for load balancing
    int assignmentsThisRound = 0;
    int maxAssignmentsPerAgent = 1; // Prevent one agent from hogging all packages
    
    // Try to assign each package
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
            
            // Assign package to agent based on type - Polymorphism
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
