#pragma once

#include <string>

namespace cycleopt {

class SubjectModel {
public:
    SubjectModel() = default;

    SubjectModel(std::string name,
                 double bodyMassKg,
                 double heightM,
                 double seatedHeightM,
                 double forearmLengthM,
                 double upperArmLengthM,
                 double thighLengthM,
                 double shankLengthM,
                 double neckLengthM,
                 double pelvisWidthM = 0.0,
                 double footLengthM = 0.0,
                 double shoulderWidthM = 0.0)
        : name_(std::move(name)),
          body_mass_kg_(bodyMassKg),
          height_m_(heightM),
          seated_height_m_(seatedHeightM),
          forearm_length_m_(forearmLengthM),
          upper_arm_length_m_(upperArmLengthM),
          thigh_length_m_(thighLengthM),
          shank_length_m_(shankLengthM),
          neck_length_m_(neckLengthM),
          pelvis_width_m_(pelvisWidthM),
          foot_length_m_(footLengthM),
          shoulder_width_m_(shoulderWidthM) {}

    SubjectModel(std::string name, double bodyMassKg, double heightM)
        : SubjectModel(std::move(name), bodyMassKg, heightM, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0) {}

    const std::string& name() const { return name_; }
    double bodyMassKg() const { return body_mass_kg_; }
    double heightM() const { return height_m_; }
    double seatedHeightM() const { return seated_height_m_; }
    double forearmLengthM() const { return forearm_length_m_; }
    double upperArmLengthM() const { return upper_arm_length_m_; }
    double thighLengthM() const { return thigh_length_m_; }
    double shankLengthM() const { return shank_length_m_; }
    double neckLengthM() const { return neck_length_m_; }
    double pelvisWidthM() const { return pelvis_width_m_; }
    double footLengthM() const { return foot_length_m_; }
    double shoulderWidthM() const { return shoulder_width_m_; }

private:
    std::string name_;
    double body_mass_kg_ = 0.0;
    double height_m_ = 0.0;
    double seated_height_m_ = 0.0;
    double forearm_length_m_ = 0.0;
    double upper_arm_length_m_ = 0.0;
    double thigh_length_m_ = 0.0;
    double shank_length_m_ = 0.0;
    double neck_length_m_ = 0.0;
    double pelvis_width_m_ = 0.0;
    double foot_length_m_ = 0.0;
    double shoulder_width_m_ = 0.0;
};

}  // namespace cycleopt
