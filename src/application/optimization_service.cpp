#include "cycleopt/application/optimization_service.hpp"
#include "cycleopt/infrastructure/open_sim_model_factory.hpp"
#include "cycleopt/infrastructure/moco_problem_translator.hpp"
#include <OpenSim/Moco/osimMoco.h>
#include <iostream>

namespace cycleopt {

OptimizationService::OptimizationService(std::vector<std::shared_ptr<ObjectiveFunction>> objectives)
    : objectives_(std::move(objectives)) {}

std::string OptimizationService::run(const SimulationScenario& scenario) const {
    std::cout << "Starting cycleOpt optimization flow...\n";

    try {
        // 1. Build and scale model
        OpenSimModelFactory modelFactory;
        std::string modelFile = modelFactory.buildModelFile(scenario);
        std::cout << "Step 1/3: Model generated -> " << modelFile << "\n";

        // 2. Setup Moco Problem
        MocoProblemTranslator translator;
        std::string problemFile = translator.translateProblem(scenario, objectives_);
        std::cout << "Step 2/3: Moco Problem defined -> " << problemFile << "\n";

        // 3. Solve (Actual execution in user environment)
        std::cout << "Step 3/3: Executing solver (IPOPT)...\n";
        OpenSim::MocoStudy study(problemFile);
        OpenSim::MocoSolution solution = study.solve();

        std::string solutionFile = "optimal_cycling_solution.sto";
        solution.write(solutionFile);

        return "Optimization completed successfully. Results saved to " + solutionFile;

    } catch (const std::exception& e) {
        std::cerr << "Error during optimization: " << e.what() << "\n";
        return std::string("Optimization failed: ") + e.what();
    }
}

}  // namespace cycleopt
