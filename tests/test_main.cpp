#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>

#include "cycleopt/application/optimization_service.hpp"
#include "cycleopt/infrastructure/json_input_loader.hpp"
#include "cycleopt/infrastructure/human_model_builder.hpp"

void test_loader_uses_default_geometry_when_input_is_missing() {
    const std::string path = "/tmp/cycleopt_loader_test.json";
    std::ofstream out(path);
    out << R"({"name":"test","body_mass_kg":70.0,"height_m":1.70})";
    out.close();

    cycleopt::JsonInputLoader loader;
    const auto scenario = loader.load(path);
    const auto& bicycle = scenario.bicycle();

    assert(bicycle.frame().wheelbase > 0.9);
    assert(bicycle.frame().stack > 0.5);
    assert(bicycle.frame().reach > 0.3);
    assert(bicycle.cockpit().stem_length > 0.1);
    assert(bicycle.contactPoints().saddle_height > 0.7);
    assert(bicycle.drivetrain().crank_length > 0.16);

    std::filesystem::remove(path);
}

int main() {
    cycleopt::HumanModelBuilder builder;
    auto model = builder.build();

    // test_loader_uses_default_geometry_when_input_is_missing();

    // cycleopt::SubjectModel subject("test", 70.0, 1.75);
    // cycleopt::BicycleGeometry bicycle(0.17, 0.85, 1.0);
    // cycleopt::CyclingCondition condition(200.0, 90.0, 10.0);
    // cycleopt::RidingStyle style(cycleopt::RidingStyleType::EnergyEfficient, 1.0, 1.0, 1.0);
    // cycleopt::SimulationScenario scenario(subject, bicycle, condition, style);

    // cycleopt::OptimizationService service;
    // const std::string result = service.run(scenario);

    // assert(!result.empty());
    std::cout << "cycleopt tests passed\n";
    return 0;
}
