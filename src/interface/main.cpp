#include <iostream>
#include <string>
#include <filesystem>

#include "cycleopt/application/optimization_service.hpp"
#include "cycleopt/infrastructure/json_input_loader.hpp"
#include "cycleopt/infrastructure/result_exporter.hpp"

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: cycleopt_cli <input.json> [output_summary.txt]\n";
        return 1;
    }

    const std::string input_path = argv[1];
    const std::string output_path = (argc >= 3) ? argv[2] : "output_summary.txt";

    if (!std::filesystem::exists(input_path)) {
        std::cerr << "Error: Input file " << input_path << " not found.\n";
        return 1;
    }

    try {
        cycleopt::JsonInputLoader loader;
        std::vector<std::shared_ptr<cycleopt::ObjectiveFunction>> objectives;

        // Default objective for cycling optimization
        objectives.push_back(std::make_shared<cycleopt::MuscleActivityObjective>());

        cycleopt::OptimizationService optimizer(std::move(objectives));
        cycleopt::ResultExporter exporter;

        std::cout << "Loading scenario from " << input_path << "...\n";
        const auto scenario = loader.load(input_path);

        const std::string result = optimizer.run(scenario);
        exporter.exportResult(output_path, result);

        std::cout << "\n--- Optimization Report ---\n";
        std::cout << result << "\n";
        std::cout << "---------------------------\n";

    } catch (const std::exception& e) {
        std::cerr << "CRITICAL ERROR: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
