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
    problem.setModelCopy(Model("cycling_model_for_moco.osim"));

    // 2. Set Time Bounds
    double cadenceRpm = scenario.condition().cadenceRpm();
    double period = 60.0 / cadenceRpm;
    problem.setTimeBounds(0, period);

    // 3. State Info
    problem.setStateInfo("/jointset/bottom_bracket/angle/value", {0, 6.3}, 0, 6.28);

    // 4. Goals from objectives
    if (objectives.empty()) {
        problem.addGoal<MocoControlGoal>("effort", 1.0);
    } else {
        for (const auto& obj : objectives) {
            auto* goal = problem.addGoal<MocoControlGoal>(obj->name());
            goal->setWeight(1.0);
        }
    }

    auto* periodicity = problem.addGoal<MocoPeriodicityGoal>("periodicity");
    for (const auto& coordinate : problem.getPhase(0).getModel().getComponentList<Coordinate>()) {
        if (coordinate.getName() != "angle") {
             periodicity->addStatePair({coordinate.getStateVariableNames()[0]});
        }
    }

    // 5. Solver
    MocoCasADiSolver& solver = study.initCasADiSolver();
    solver.set_num_mesh_intervals(25);
    solver.set_optim_solver("ipopt");
    solver.set_optim_convergence_tolerance(1e-2);

    std::string problemFile = "cycling_problem.omoco";
    study.print(problemFile);

    return problemFile;
}

}  // namespace cycleopt
