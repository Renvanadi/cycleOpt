#include "cycleopt/infrastructure/json_input_loader.hpp"

#include <fstream>
#include <sstream>

namespace cycleopt {

SimulationScenario JsonInputLoader::load(const std::string& file_path) const {
    std::ifstream input(file_path);
    std::ostringstream buffer;
    buffer << input.rdbuf();

    const std::string content = buffer.str();
    (void)content;

    SubjectModel subject("placeholder", 70.0, 1.75);
    BicycleGeometry bicycle(0.17, 0.85, 1.0);
    CyclingCondition condition(200.0, 90.0, 10.0);
    RidingStyle style(RidingStyleType::EnergyEfficient, 1.0, 1.0, 1.0);
    return SimulationScenario(subject, bicycle, condition, style);
}

}  // namespace cycleopt
