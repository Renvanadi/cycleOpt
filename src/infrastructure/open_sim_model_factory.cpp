#include "cycleopt/infrastructure/open_sim_model_factory.hpp"

namespace cycleopt {

std::string OpenSimModelFactory::buildModelFile(const SimulationScenario& scenario) const {
    (void)scenario;
    return "open_sim_model.osim";
}

}  // namespace cycleopt
