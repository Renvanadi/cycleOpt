#pragma once

#include <string>

#include "cycleopt/domain/simulation_scenario.hpp"

namespace cycleopt {

class OpenSimModelFactory {
public:
    OpenSimModelFactory() = default;
    std::string buildModelFile(const SimulationScenario& scenario) const;
};

}  // namespace cycleopt
