#pragma once

namespace cycleopt {

class CyclingCondition {
public:
    CyclingCondition() = default;

    CyclingCondition(double powerWatts, double cadenceRpm, double speedMps)
        : power_watts_(powerWatts), cadence_rpm_(cadenceRpm), speed_mps_(speedMps) {}

    double powerWatts() const { return power_watts_; }
    double cadenceRpm() const { return cadence_rpm_; }
    double speedMps() const { return speed_mps_; }

private:
    double power_watts_ = 0.0;
    double cadence_rpm_ = 0.0;
    double speed_mps_ = 0.0;
};

}  // namespace cycleopt
