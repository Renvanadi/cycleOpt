#include "cycleopt/infrastructure/moco_problem_translator.hpp"
#include <OpenSim/Moco/osimMoco.h>

namespace cycleopt {

std::string MocoProblemTranslator::translateProblem(
    const SimulationScenario& scenario,
    const std::vector<std::shared_ptr<ObjectiveFunction>>& objectives) const {

    using namespace OpenSim;

    MocoStudy study;
    study.setName("cycling_optimization");

    MocoProblem& problem = study.updProblem();

    // 1. Set Model
    Model model("final_cycling_model.osim");
    problem.setModelCopy(model);

    // 2. Set Time Bounds
    double cadenceRpm = scenario.condition().cadenceRpm();
    double period = 60.0 / cadenceRpm;
    problem.setTimeBounds(0, period);

    // 3. Coordinate Bounds and Periodicity
    auto& mocoModel = problem.updModel();
    // Ensure the crank starts at 0 and ends at 2*pi
    problem.setStateInfo("/jointset/bottom_bracket/angle/value", {0, 6.3}, 0, 6.28);

    // 4. Goals
    auto* effort = problem.addGoal<MocoControlGoal>("effort");
    effort->setWeight(1.0);

    auto* periodicity = problem.addGoal<MocoPeriodicityGoal>("periodicity");
    for (const auto& coordinate : model.getComponentList<Coordinate>()) {
        if (coordinate.getName() != "angle") { // bottom_bracket angle is not periodic in value but in state
             periodicity->addStatePair({coordinate.getStateVariableNames()[0]});
        }
    }

    // 5. Solver Setup
    MocoCasADiSolver& solver = study.initCasADiSolver();
    solver.set_num_mesh_intervals(25);
    solver.set_optim_solver("ipopt");
    solver.set_optim_convergence_tolerance(1e-2);

    std::string problemFile = "cycling_problem.omoco";
    study.print(problemFile);

    return problemFile;
}

}  // namespace cycleopt
