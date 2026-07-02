#pragma once

#include <string>

#include "cycleopt/domain/simulation_scenario.hpp"

namespace cycleopt {

class JsonInputLoader {
public:
    SimulationScenario load(const std::string& file_path) const;
};

}  // namespace cycleopt
