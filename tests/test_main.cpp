#include <cassert>
#include <iostream>

#include "cycleopt/application/optimization_service.hpp"

int main() {
    cycleopt::SubjectModel subject("test", 70.0, 1.75);
    cycleopt::BicycleGeometry bicycle(0.17, 0.85, 1.0);
    cycleopt::CyclingCondition condition(200.0, 90.0, 10.0);
    cycleopt::RidingStyle style(cycleopt::RidingStyleType::EnergyEfficient, 1.0, 1.0, 1.0);
    cycleopt::SimulationScenario scenario(subject, bicycle, condition, style);

    cycleopt::OptimizationService service;
    const std::string result = service.run(scenario);

    assert(!result.empty());
    std::cout << "cycleopt tests passed\n";
    return 0;
}
