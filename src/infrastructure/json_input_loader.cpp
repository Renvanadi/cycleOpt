#include "cycleopt/infrastructure/json_input_loader.hpp"
#include <nlohmann/json.hpp>
#include <fstream>

using json = nlohmann::json;

namespace cycleopt {

SimulationScenario JsonInputLoader::load(const std::string& file_path) const {
    std::ifstream input(file_path);
    json j;
    input >> j;

    SubjectModel subject(
        j.value("name", "unknown"),
        j.value("body_mass_kg", 70.0),
        j.value("height_m", 1.75),
        j.value("seated_height_m", 0.0),
        j.value("forearm_length_m", 0.0),
        j.value("upper_arm_length_m", 0.0),
        j.value("thigh_length_m", 0.0),
        j.value("shank_length_m", 0.0),
        j.value("neck_length_m", 0.0),
        j.value("pelvis_width_m", 0.0),
        j.value("foot_length_m", 0.0),
        j.value("shoulder_width_m", 0.0)
    );

    BicycleGeometry::FrameGeometry frame;
    if (j.contains("bicycle") && j["bicycle"].contains("frame")) {
        auto& f = j["bicycle"]["frame"];
        frame.wheelbase = f.value("wheelbase", 1.02);
        frame.chainstay_length = f.value("chainstay_length", 0.42);
        frame.seat_tube_length = f.value("seat_tube_length", 0.50);
        frame.top_tube_length = f.value("top_tube_length", 0.56);
        frame.effective_top_tube_length = f.value("effective_top_tube_length", 0.54);
        frame.down_tube_length = f.value("down_tube_length", 0.68);
        frame.head_tube_length = f.value("head_tube_length", 0.15);
        frame.seat_tube_angle = f.value("seat_tube_angle", 73.0);
        frame.head_tube_angle = f.value("head_tube_angle", 73.0);
        frame.bottom_bracket_drop = f.value("bottom_bracket_drop", 0.07);
        frame.bottom_bracket_height = f.value("bottom_bracket_height", 0.28);
        frame.fork_length = f.value("fork_length", 0.37);
        frame.fork_offset = f.value("fork_offset", 0.045);
        frame.front_center = f.value("front_center", 0.39);
        frame.rear_center = f.value("rear_center", 0.42);
        frame.stack = f.value("stack", 0.56);
        frame.reach = f.value("reach", 0.39);
    } else {
        frame.wheelbase = 1.02;
        frame.chainstay_length = 0.42;
        frame.seat_tube_length = 0.50;
        frame.top_tube_length = 0.56;
        frame.effective_top_tube_length = 0.54;
        frame.down_tube_length = 0.68;
        frame.head_tube_length = 0.15;
        frame.seat_tube_angle = 73.0;
        frame.head_tube_angle = 73.0;
        frame.bottom_bracket_drop = 0.07;
        frame.bottom_bracket_height = 0.28;
        frame.fork_length = 0.37;
        frame.fork_offset = 0.045;
        frame.front_center = 0.39;
        frame.rear_center = 0.42;
        frame.stack = 0.56;
        frame.reach = 0.39;
    }

    BicycleGeometry::Cockpit cockpit;
    if (j.contains("bicycle") && j["bicycle"].contains("cockpit")) {
        auto& c = j["bicycle"]["cockpit"];
        cockpit.stem_length = c.value("stem_length", 0.12);
        cockpit.stem_angle = c.value("stem_angle", -6.0);
        cockpit.handlebar_width = c.value("handlebar_width", 0.44);
        cockpit.handlebar_reach = c.value("handlebar_reach", 0.08);
        cockpit.handlebar_drop = c.value("handlebar_drop", 0.06);
    } else {
        cockpit.stem_length = 0.12;
        cockpit.stem_angle = -6.0;
        cockpit.handlebar_width = 0.44;
        cockpit.handlebar_reach = 0.08;
        cockpit.handlebar_drop = 0.06;
    }

    BicycleGeometry::Drivetrain drivetrain;
    if (j.contains("bicycle") && j["bicycle"].contains("drivetrain")) {
        auto& d = j["bicycle"]["drivetrain"];
        drivetrain.crank_length = d.value("crank_length", 0.172);
        drivetrain.q_factor = d.value("q_factor", 0.146);
        drivetrain.pedal_spindle_length = d.value("pedal_spindle_length", 0.015);
    } else {
        drivetrain.crank_length = 0.172;
        drivetrain.q_factor = 0.146;
        drivetrain.pedal_spindle_length = 0.015;
    }

    BicycleGeometry::WheelTire wheelTire;
    if (j.contains("bicycle") && j["bicycle"].contains("wheel_tire")) {
        auto& wt = j["bicycle"]["wheel_tire"];
        wheelTire.wheel_radius = wt.value("wheel_radius", 0.345);
        wheelTire.tire_radius = wt.value("tire_radius", 0.345);
    } else {
        wheelTire.wheel_radius = 0.345;
        wheelTire.tire_radius = 0.345;
    }

    BicycleGeometry::ContactPoints contactPoints;
    if (j.contains("bicycle") && j["bicycle"].contains("contact_points")) {
        auto& cp = j["bicycle"]["contact_points"];
        contactPoints.saddle_height = cp.value("saddle_height", 0.78);
        contactPoints.saddle_setback = cp.value("saddle_setback", 0.02);
        contactPoints.saddle_tilt = cp.value("saddle_tilt", 0.0);
        contactPoints.hood_position = cp.value("hood_position", 0.02);
        contactPoints.hand_position = cp.value("hand_position", 0.02);

        if (cp.contains("saddle_position")) {
            contactPoints.saddle_position.x = cp["saddle_position"].value("x", 0.0);
            contactPoints.saddle_position.y = cp["saddle_position"].value("y", 0.0);
            contactPoints.saddle_position.z = cp["saddle_position"].value("z", 0.0);
        }
        if (cp.contains("bottom_bracket_position")) {
            contactPoints.bottom_bracket_position.x = cp["bottom_bracket_position"].value("x", 0.0);
            contactPoints.bottom_bracket_position.y = cp["bottom_bracket_position"].value("y", 0.0);
            contactPoints.bottom_bracket_position.z = cp["bottom_bracket_position"].value("z", 0.0);
        }
        if (cp.contains("handlebar_position")) {
            contactPoints.handlebar_position.x = cp["handlebar_position"].value("x", 0.0);
            contactPoints.handlebar_position.y = cp["handlebar_position"].value("y", 0.0);
            contactPoints.handlebar_position.z = cp["handlebar_position"].value("z", 0.0);
        }
    } else {
        contactPoints.saddle_height = 0.78;
        contactPoints.saddle_setback = 0.02;
        contactPoints.saddle_tilt = 0.0;
        contactPoints.hood_position = 0.02;
        contactPoints.hand_position = 0.02;
    }

    BicycleGeometry bicycle(frame, cockpit, drivetrain, wheelTire, contactPoints);

    CyclingCondition condition;
    if (j.contains("cycling_condition")) {
        auto& cc = j["cycling_condition"];
        condition = CyclingCondition(
            cc.value("power_watts", 200.0),
            cc.value("cadence_rpm", 90.0),
            cc.value("speed_mps", 10.0)
        );
    }

    RidingStyle style;
    if (j.contains("riding_style")) {
        auto& rs = j["riding_style"];
        style = RidingStyle(
            static_cast<RidingStyleType>(rs.value("type", 0)),
            rs.value("muscle_weight", 1.0),
            rs.value("metabolic_weight", 1.0),
            rs.value("volume_weight", 1.0)
        );
    }

    return SimulationScenario(subject, bicycle, condition, style);
}

}  // namespace cycleopt
