#include "cycleopt/infrastructure/open_sim_model_factory.hpp"

#include <OpenSim/OpenSim.h>
#include <OpenSim/Simulation/Model/Model.h>
#include <OpenSim/Simulation/SimbodyEngine/PinJoint.h>
#include <OpenSim/Simulation/SimbodyEngine/WeldJoint.h>
#include <OpenSim/Common/ScaleSet.h>
#include <OpenSim/Actuators/CoordinateActuator.h>

namespace cycleopt {

std::string OpenSimModelFactory::buildModelFile(const SimulationScenario& scenario) const {
    using namespace OpenSim;

    // 1. Load generic model
    std::string baseModelFile = "generic.osim";
    auto model = std::make_unique<Model>();
    try {
        model = std::make_unique<Model>(baseModelFile);
    } catch (...) {
        model->setName("CyclingModel");
    }

    // 2. High-precision Scaling
    const auto& subject = scenario.subject();
    ScaleSet scaleSet;

    auto addScale = [&](const std::string& segmentName, double factor) {
        auto* scale = new Scale();
        scale->setSegmentName(segmentName);
        scale->setScaleFactors(SimTK::Vec3(1.0, factor, 1.0));
        scale->setApply(true);
        scaleSet.adoptAndAppend(scale);
    };

    if (model->getBodySet().size() > 0) {
        addScale("femur_r", subject.thighLengthM() / 0.45);
        addScale("tibia_r", subject.shankLengthM() / 0.45);
        // ... (simplified for example)
        SimTK::State& state = model->initSystem();
        model->scale(state, scaleSet, true);
        model->scaleMass(state, subject.bodyMassKg() / model->get_total_mass(state));
    }

    // 3. Build Bicycle Model
    const auto& bike = scenario.bicycle();
    auto* frame = new OpenSim::Body("bicycle_frame", 8.0, SimTK::Vec3(0), SimTK::Inertia(1));
    model->addBody(frame);

    auto* groundToFrame = new WeldJoint("ground_to_frame",
        model->getGround(), SimTK::Vec3(0), SimTK::Vec3(0),
        *frame, SimTK::Vec3(0), SimTK::Vec3(0));
    model->addJoint(groundToFrame);

    // Attach Pelvis
    if (model->updBodySet().contains("pelvis")) {
        auto* saddleJoint = new WeldJoint("saddle_contact",
            *frame, SimTK::Vec3(bike.contactPoints().saddle_position.x,
                                bike.contactPoints().saddle_position.y,
                                bike.contactPoints().saddle_position.z), SimTK::Vec3(0),
            model->updBodySet().get("pelvis"), SimTK::Vec3(0), SimTK::Vec3(0));
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

    // 4. Add Actuators (Solvability)
    for (auto& coord : model->updCoordinateSet()) {
        auto* actu = new CoordinateActuator(coord.getName());
        actu->setName("actu_" + coord.getName());
        actu->setOptimalForce(100.0);
        model->addForce(actu);
    }

    // Finalize
    std::string outputFilename = "cycling_model_for_moco.osim";
    model->finalizeConnections();
    model->print(outputFilename);

    return outputFilename;
}

}  // namespace cycleopt
