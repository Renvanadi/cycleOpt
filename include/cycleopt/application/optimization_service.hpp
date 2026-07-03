#pragma once

#include <memory>
#include <string>
#include <vector>

#include "cycleopt/application/objective_function.hpp"
#include "cycleopt/domain/simulation_scenario.hpp"

namespace cycleopt {

class OptimizationService {
public:
    explicit OptimizationService(std::vector<std::shared_ptr<ObjectiveFunction>> objectives);
    std::string run(const SimulationScenario& scenario) const;

private:
    std::vector<std::shared_ptr<ObjectiveFunction>> objectives_;
};

}  // namespace cycleopt
