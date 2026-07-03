#include <iostream>
#include <string>

#include "cycleopt/application/optimization_service.hpp"
#include "cycleopt/infrastructure/json_input_loader.hpp"
#include "cycleopt/infrastructure/result_exporter.hpp"

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: cycleopt_cli <input.json> [output.txt]\n";
        return 1;
    }

    const std::string input_path = argv[1];
    const std::string output_path = (argc >= 3) ? argv[2] : "output.txt";

    cycleopt::JsonInputLoader loader;
    std::vector<std::shared_ptr<cycleopt::ObjectiveFunction>> objectives;
    objectives.push_back(std::make_shared<cycleopt::MuscleActivityObjective>());
    objectives.push_back(std::make_shared<cycleopt::MetabolicEnergyObjective>());
    objectives.push_back(std::make_shared<cycleopt::VolumeWeightedActivityObjective>());

    cycleopt::OptimizationService optimizer(std::move(objectives));
    cycleopt::ResultExporter exporter;

    const auto scenario = loader.load(input_path);
    const std::string result = optimizer.run(scenario);
    exporter.exportResult(output_path, result);

    std::cout << result << "\n";
    return 0;
}
