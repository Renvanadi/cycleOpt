#include "cycleopt/application/optimization_service.hpp"

#include <sstream>

namespace cycleopt {

OptimizationService::OptimizationService(std::vector<std::shared_ptr<ObjectiveFunction>> objectives)
    : objectives_(std::move(objectives)) {}

std::string OptimizationService::run(const SimulationScenario& scenario) const {
    std::ostringstream oss;
    oss << "Optimization requested for " << scenario.subject().name()
        << ", power=" << scenario.condition().powerWatts()
        << ", cadence=" << scenario.condition().cadenceRpm() << "\n";

    for (const auto& objective : objectives_) {
        oss << objective->name() << " score=" << objective->evaluate(scenario) << "\n";
    }

    return oss.str();
}

}  // namespace cycleopt
