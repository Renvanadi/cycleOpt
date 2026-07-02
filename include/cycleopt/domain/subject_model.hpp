#pragma once

#include <string>

namespace cycleopt {

class SubjectModel {
public:
    SubjectModel() = default;

    SubjectModel(std::string name, double bodyMassKg, double heightM)
        : name_(std::move(name)), body_mass_kg_(bodyMassKg), height_m_(heightM) {}

    const std::string& name() const { return name_; }
    double bodyMassKg() const { return body_mass_kg_; }
    double heightM() const { return height_m_; }

private:
    std::string name_;
    double body_mass_kg_ = 0.0;
    double height_m_ = 0.0;
};

}  // namespace cycleopt
