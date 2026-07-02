#pragma once

#include <string>

#include "cycleopt/domain/simulation_scenario.hpp"

namespace cycleopt {

class OptimizationService {
public:
    std::string run(const SimulationScenario& scenario) const;
};

}  // namespace cycleopt
