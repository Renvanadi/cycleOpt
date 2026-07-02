#pragma once

#include <string>

namespace cycleopt {

enum class RidingStyleType {
    EnergyEfficient,
    LargeMuscleGroup,
    UpperBodyFocus
};

class RidingStyle {
public:
    RidingStyle() = default;

    RidingStyle(RidingStyleType type, double muscleWeight, double metabolicWeight, double volumeWeight)
        : type_(type), muscle_weight_(muscleWeight), metabolic_weight_(metabolicWeight), volume_weight_(volumeWeight) {}

    RidingStyleType type() const { return type_; }
    double muscleWeight() const { return muscle_weight_; }
    double metabolicWeight() const { return metabolic_weight_; }
    double volumeWeight() const { return volume_weight_; }

private:
    RidingStyleType type_ = RidingStyleType::EnergyEfficient;
    double muscle_weight_ = 0.0;
    double metabolic_weight_ = 0.0;
    double volume_weight_ = 0.0;
};

}  // namespace cycleopt
