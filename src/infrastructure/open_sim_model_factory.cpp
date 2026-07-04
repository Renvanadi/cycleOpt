#include "cycleopt/infrastructure/open_sim_model_factory.hpp"

#include <OpenSim/OpenSim.h>
#include <OpenSim/Simulation/Model/Model.h>
#include <OpenSim/Simulation/SimbodyEngine/PinJoint.h>
#include <OpenSim/Simulation/SimbodyEngine/WeldJoint.h>
#include <OpenSim/Common/ScaleSet.h>
#include <OpenSim/Tools/ScaleTool.h>

namespace cycleopt {

std::string OpenSimModelFactory::buildModelFile(const SimulationScenario& scenario) const {
    using namespace OpenSim;

    // 1. Load generic model
    std::string baseModelFile = "generic.osim";
    auto model = std::make_unique<Model>(baseModelFile);
    model->workingState(); // Ensure the model is initialized

    // 2. High-precision Scaling using ScaleSet and model->scale()
    const auto& subject = scenario.subject();

    ScaleSet scaleSet;

    auto addScale = [&](const std::string& segmentName, double factor) {
        auto* scale = new Scale();
        scale->setSegmentName(segmentName);
        scale->setScaleFactors(SimTK::Vec3(1.0, factor, 1.0)); // Assuming Y is the length direction
        scale->setApply(true);
        scaleSet.adoptAndAppend(scale);
    };

    // Calculate scale factors based on subject vs generic model (assumed defaults)
    addScale("femur_r", subject.thighLengthM() / 0.45);
    addScale("femur_l", subject.thighLengthM() / 0.45);
    addScale("tibia_r", subject.shankLengthM() / 0.45);
    addScale("tibia_l", subject.shankLengthM() / 0.45);
    addScale("humerus_r", subject.upperArmLengthM() / 0.30);
    addScale("humerus_l", subject.upperArmLengthM() / 0.30);
    addScale("radius_r", subject.forearmLengthM() / 0.25);
    addScale("radius_l", subject.forearmLengthM() / 0.25);
    // ... add more segments as needed ...

    // Apply scaling
    // This updates body dimensions, joint offsets, and muscle points
    SimTK::State& state = model->initSystem();
    model->scale(state, scaleSet, true);

    // Update mass based on total body mass
    double totalMass = subject.bodyMassKg();
    double modelMass = model->get_total_mass(state);
    model->scaleMass(state, totalMass / modelMass);

    // 3. Build Bicycle Model and Attach Rider
    const auto& bike = scenario.bicycle();

    auto* frame = new OpenSim::Body("bicycle_frame", 8.0, SimTK::Vec3(0), SimTK::Inertia(1));
    model->addBody(frame);

    auto* groundToFrame = new WeldJoint("ground_to_frame",
        model->getGround(), SimTK::Vec3(0), SimTK::Vec3(0),
        *frame, SimTK::Vec3(0), SimTK::Vec3(0));
    model->addJoint(groundToFrame);

    // Attach Rider Pelvis to Saddle
    auto& bodySet = model->updBodySet();
    if (bodySet.contains("pelvis")) {
        auto& pelvis = bodySet.get("pelvis");
        auto* saddleJoint = new WeldJoint("saddle_contact",
            *frame, SimTK::Vec3(bike.contactPoints().saddle_position.x,
                                bike.contactPoints().saddle_position.y,
                                bike.contactPoints().saddle_position.z), SimTK::Vec3(0),
            pelvis, SimTK::Vec3(0), SimTK::Vec3(0));
        model->addJoint(saddleJoint);
    }

    // Add Drivetrain
    auto* crank = new OpenSim::Body("crank", 0.4, SimTK::Vec3(0), SimTK::Inertia(0.01));
    model->addBody(crank);

    auto* bbJoint = new PinJoint("bottom_bracket",
        *frame, SimTK::Vec3(bike.contactPoints().bottom_bracket_position.x,
                            bike.contactPoints().bottom_bracket_position.y,
                            bike.contactPoints().bottom_bracket_position.z), SimTK::Vec3(0, 0, 1.57),
        *crank, SimTK::Vec3(0), SimTK::Vec3(0));
    model->addJoint(bbJoint);

    // Finalize
    std::string outputFilename = "scaled_bicycle_rider_model.osim";
    model->finalizeConnections();
    model->print(outputFilename);

    return outputFilename;
}

}  // namespace cycleopt
