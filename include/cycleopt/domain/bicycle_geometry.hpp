#pragma once

#include "cycleopt/domain/point3d.hpp"

namespace cycleopt {

class BicycleGeometry {
public:
    BicycleGeometry() = default;

    struct FrameGeometry {
        double wheelbase = 0.0;
        double chainstay_length = 0.0;
        double seat_tube_length = 0.0;
        double top_tube_length = 0.0;
        double effective_top_tube_length = 0.0;
        double down_tube_length = 0.0;
        double head_tube_length = 0.0;
        double seat_tube_angle = 0.0; // degrees
        double head_tube_angle = 0.0; // degrees
        double bottom_bracket_drop = 0.0;
        double bottom_bracket_height = 0.0;
        double fork_length = 0.0;
        double fork_offset = 0.0;
        double front_center = 0.0;
        double rear_center = 0.0;
        double stack = 0.0;
        double reach = 0.0;
    };

    struct Cockpit {
        double stem_length = 0.0;
        double stem_angle = 0.0;
        double handlebar_width = 0.0;
        double handlebar_reach = 0.0;
        double handlebar_drop = 0.0;
    };

    struct Drivetrain {
        double crank_length = 0.0;
        double q_factor = 0.0;
        double pedal_spindle_length = 0.0;
    };

    struct WheelTire {
        double wheel_radius = 0.0;
        double tire_radius = 0.0;
    };

    struct ContactPoints {
        double saddle_height = 0.0;
        double saddle_setback = 0.0;
        double saddle_tilt = 0.0;
        double hood_position = 0.0;
        double hand_position = 0.0;
        Point3D saddle_position;
        Point3D bottom_bracket_position;
        Point3D handlebar_position;
    };

    BicycleGeometry(const FrameGeometry& frame,
                    const Cockpit& cockpit,
                    const Drivetrain& drivetrain,
                    const WheelTire& wheelTire,
                    const ContactPoints& contactPoints)
        : frame_(frame),
          cockpit_(cockpit),
          drivetrain_(drivetrain),
          wheel_tire_(wheelTire),
          contact_points_(contactPoints) {}

    // Legacy constructor for backward compatibility
    BicycleGeometry(double crankLengthM,
                    double saddleHeightM,
                    double handlebarHeightM) {
        drivetrain_.crank_length = crankLengthM;
        contact_points_.saddle_height = saddleHeightM;
        // Approximation for handlebar height if needed, or just set it
    }

    const FrameGeometry& frame() const { return frame_; }
    const Cockpit& cockpit() const { return cockpit_; }
    const Drivetrain& drivetrain() const { return drivetrain_; }
    const WheelTire& wheelTire() const { return wheel_tire_; }
    const ContactPoints& contactPoints() const { return contact_points_; }

    // Legacy getters
    double crankLengthM() const { return drivetrain_.crank_length; }
    double saddleHeightM() const { return contact_points_.saddle_height; }
    double qFactorM() const { return drivetrain_.q_factor; }
    double handlebarWidthM() const { return cockpit_.handlebar_width; }
    const Point3D& saddlePosition() const { return contact_points_.saddle_position; }
    const Point3D& bottomBracketPosition() const { return contact_points_.bottom_bracket_position; }
    const Point3D& handlebarPosition() const { return contact_points_.handlebar_position; }

private:
    FrameGeometry frame_;
    Cockpit cockpit_;
    Drivetrain drivetrain_;
    WheelTire wheel_tire_;
    ContactPoints contact_points_;
};

}  // namespace cycleopt
