#pragma once

#include "cycleopt/domain/simulation_scenario.hpp"

namespace cycleopt {

class ObjectiveFunction {
public:
    virtual ~ObjectiveFunction() = default;
    virtual double evaluate(const SimulationScenario& scenario) const = 0;
    virtual std::string name() const = 0;
};

class MuscleActivityObjective : public ObjectiveFunction {
public:
    double evaluate(const SimulationScenario& scenario) const override;
    std::string name() const override { return "MuscleActivity"; }
};

class MetabolicEnergyObjective : public ObjectiveFunction {
public:
    double evaluate(const SimulationScenario& scenario) const override;
    std::string name() const override { return "MetabolicEnergy"; }
};

class VolumeWeightedActivityObjective : public ObjectiveFunction {
public:
    double evaluate(const SimulationScenario& scenario) const override;
    std::string name() const override { return "VolumeWeightedActivity"; }
};

}  // namespace cycleopt
