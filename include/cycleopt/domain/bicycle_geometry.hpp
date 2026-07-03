#pragma once

#include "cycleopt/domain/point3d.hpp"

namespace cycleopt {

class BicycleGeometry {
public:
    BicycleGeometry() = default;

    BicycleGeometry(double crankLengthM,
                    double saddleHeightM,
                    double handlebarHeightM,
                    Point3D saddlePosition,
                    Point3D bottomBracketPosition,
                    Point3D handlebarPosition,
                    double qFactorM,
                    double handlebarWidthM)
        : crank_length_m_(crankLengthM),
          saddle_height_m_(saddleHeightM),
          handlebar_height_m_(handlebarHeightM),
          saddle_position_(saddlePosition),
          bottom_bracket_position_(bottomBracketPosition),
          handlebar_position_(handlebarPosition),
          q_factor_m_(qFactorM),
          handlebar_width_m_(handlebarWidthM) {}

    BicycleGeometry(double crankLengthM,
                    double saddleHeightM,
                    double handlebarHeightM)
        : BicycleGeometry(crankLengthM,
                         saddleHeightM,
                         handlebarHeightM,
                         Point3D{0.0, 0.0, 0.0},
                         Point3D{0.0, 0.0, 0.0},
                         Point3D{0.0, 0.0, 0.0},
                         0.0,
                         0.0) {}

    double crankLengthM() const { return crank_length_m_; }
    double saddleHeightM() const { return saddle_height_m_; }
    double handlebarHeightM() const { return handlebar_height_m_; }
    const Point3D& saddlePosition() const { return saddle_position_; }
    const Point3D& bottomBracketPosition() const { return bottom_bracket_position_; }
    const Point3D& handlebarPosition() const { return handlebar_position_; }
    double qFactorM() const { return q_factor_m_; }
    double handlebarWidthM() const { return handlebar_width_m_; }

private:
    double crank_length_m_ = 0.0;
    double saddle_height_m_ = 0.0;
    double handlebar_height_m_ = 0.0;
    Point3D saddle_position_;
    Point3D bottom_bracket_position_;
    Point3D handlebar_position_;
    double q_factor_m_ = 0.0;
    double handlebar_width_m_ = 0.0;
};

}  // namespace cycleopt
