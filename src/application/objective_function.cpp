#include "cycleopt/application/objective_function.hpp"

namespace cycleopt {

double MuscleActivityObjective::evaluate(const SimulationScenario& scenario) const {
    (void)scenario;
    return 1.0;
}

double MetabolicEnergyObjective::evaluate(const SimulationScenario& scenario) const {
    (void)scenario;
    return 1.0;
}

double VolumeWeightedActivityObjective::evaluate(const SimulationScenario& scenario) const {
    (void)scenario;
    return 1.0;
}

}  // namespace cycleopt
