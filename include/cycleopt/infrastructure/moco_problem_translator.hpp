#pragma once

#include <memory>
#include <string>
#include <vector>

#include "cycleopt/domain/simulation_scenario.hpp"
#include "cycleopt/application/objective_function.hpp"

namespace cycleopt {

class MocoProblemTranslator {
public:
    MocoProblemTranslator() = default;
    std::string translateProblem(const SimulationScenario& scenario,
                                 const std::vector<std::shared_ptr<ObjectiveFunction>>& objectives) const;
};

}  // namespace cycleopt
