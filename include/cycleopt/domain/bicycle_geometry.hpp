#pragma once

namespace cycleopt {

class BicycleGeometry {
public:
    BicycleGeometry() = default;

    BicycleGeometry(double crankLengthM, double saddleHeightM, double handlebarHeightM)
        : crank_length_m_(crankLengthM), saddle_height_m_(saddleHeightM), handlebar_height_m_(handlebarHeightM) {}

    double crankLengthM() const { return crank_length_m_; }
    double saddleHeightM() const { return saddle_height_m_; }
    double handlebarHeightM() const { return handlebar_height_m_; }

private:
    double crank_length_m_ = 0.0;
    double saddle_height_m_ = 0.0;
    double handlebar_height_m_ = 0.0;
};

}  // namespace cycleopt
