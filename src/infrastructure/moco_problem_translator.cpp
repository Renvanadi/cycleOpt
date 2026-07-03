#include "cycleopt/infrastructure/moco_problem_translator.hpp"

namespace cycleopt {

std::string MocoProblemTranslator::translateProblem(
    const SimulationScenario& scenario,
    const std::vector<std::shared_ptr<ObjectiveFunction>>& objectives) const {
    (void)scenario;
    (void)objectives;
    return "moco_problem.moco";
}

}  // namespace cycleopt
