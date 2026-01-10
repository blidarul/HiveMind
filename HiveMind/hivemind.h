#pragma once
#include "agent.h"
#include "map.h"
#include "package.h"
#include <memory>
#include <vector>

class HiveMind
{
public:
    explicit HiveMind(const Map& map);
    
    // Core algorithm: assigns packages to best available agents
    void assignPackages(   
        std::vector<std::unique_ptr<Package>>& packagesInBase, 
        std::vector<std::unique_ptr<IAgent>>& agents);
    
private:
    const Map& m_map;
    
    // helper functions
    int calculateDistance(mapPosition from, mapPosition to) const;
    int calculateTripCost(IAgent* agent, const Package& package) const;
    bool canCompleteDelivery(IAgent* agent, const Package& package) const;
    bool needsCharging(IAgent* agent, const Package& package) const;
    mapPosition findNearestChargingStation(mapPosition from) const;
    int calculateBatteryNeeded(IAgent* agent, mapPosition from, mapPosition to) const;
    
    IAgent* selectBestAgent(
        const Package& package,
        std::vector<std::unique_ptr<IAgent>>& agents);
    
    float calculateAgentScore(
        IAgent* agent,
        const Package& package,
        int distance) const;
};
