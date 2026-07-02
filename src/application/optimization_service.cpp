#include "cycleopt/application/optimization_service.hpp"

#include <sstream>

namespace cycleopt {

std::string OptimizationService::run(const SimulationScenario& scenario) const {
    std::ostringstream oss;
    oss << "Optimization requested for " << scenario.subject().name()
        << ", power=" << scenario.condition().powerWatts()
        << ", cadence=" << scenario.condition().cadenceRpm();
    return oss.str();
}

}  // namespace cycleopt
