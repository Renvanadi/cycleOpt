#pragma once

#include "cycleopt/domain/bicycle_geometry.hpp"
#include "cycleopt/domain/cycling_condition.hpp"
#include "cycleopt/domain/riding_style.hpp"
#include "cycleopt/domain/subject_model.hpp"

namespace cycleopt {

class SimulationScenario {
public:
    SimulationScenario() = default;

    SimulationScenario(SubjectModel subject, BicycleGeometry bicycle, CyclingCondition condition, RidingStyle style)
        : subject_(std::move(subject)), bicycle_(std::move(bicycle)), condition_(std::move(condition)), style_(std::move(style)) {}

    const SubjectModel& subject() const { return subject_; }
    const BicycleGeometry& bicycle() const { return bicycle_; }
    const CyclingCondition& condition() const { return condition_; }
    const RidingStyle& style() const { return style_; }

private:
    SubjectModel subject_;
    BicycleGeometry bicycle_;
    CyclingCondition condition_;
    RidingStyle style_;
};

}  // namespace cycleopt
