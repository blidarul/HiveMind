#pragma once
#include "agent.h"
#include "package.h"
#include "map.h"
#include <vector>
#include <memory>

// Strategy Pattern: HiveMind algorithm for package assignment
class HiveMind
{
public:
    explicit HiveMind(const Map& map);
    
    // Core algorithm: assigns packages to best available agents
    void assignPackages(
        std::vector<std::unique_ptr<Package>>& packagesInBase,
        std::vector<std::unique_ptr<IAgent>>& agents
    );
    
private:
    const Map& m_map;
    
    // Helper functions for decision making
    int calculateDistance(mapPosition from, mapPosition to) const;
    int calculateTripCost(IAgent* agent, const Package& package) const;
    bool canCompleteDelivery(IAgent* agent, const Package& package) const;
    bool needsCharging(IAgent* agent, const Package& package) const;
    mapPosition findNearestChargingStation(mapPosition from) const;
    int calculateBatteryNeeded(IAgent* agent, mapPosition from, mapPosition to) const;
    
    IAgent* selectBestAgent(
        const Package& package,
        std::vector<std::unique_ptr<IAgent>>& agents
    );
    
    // Scoring function for agent selection
    float calculateAgentScore(
        IAgent* agent,
        const Package& package,
        int distance
    ) const;
};
