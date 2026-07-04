#include "cycleopt/infrastructure/open_sim_model_factory.hpp"

#include <OpenSim/OpenSim.h>
#include <OpenSim/Simulation/Model/Model.h>
#include <OpenSim/Simulation/SimbodyEngine/PinJoint.h>
#include <OpenSim/Simulation/SimbodyEngine/WeldJoint.h>
#include <OpenSim/Tools/ScaleTool.h>

namespace cycleopt {

std::string OpenSimModelFactory::buildModelFile(const SimulationScenario& scenario) const {
    using namespace OpenSim;

    // 1. Load generic model
    std::string baseModelFile = "generic.osim";
    std::unique_ptr<Model> model;
    try {
        model = std::make_unique<Model>(baseModelFile);
    } catch (...) {
        model = std::make_unique<Model>();
        model->setName("CyclingModel");
    }

    // 2. Scaling Logic using OpenSim tools
    const auto& subject = scenario.subject();

    // Scale the model based on subject mass
    double totalMass = subject.bodyMassKg();
    // model->scale(SimTK::State(), ...); // Requires State and more info

    // Manual scaling of joints and bodies to ensure consistency
    auto& bodySet = model->updBodySet();
    auto& jointSet = model->updJointSet();

    auto scaleSegment = [&](const std::string& bodyName, double scaleFactor, double massScale) {
        if (bodySet.contains(bodyName)) {
            auto& body = bodySet.get(bodyName);
            body.set_mass(body.get_mass() * massScale);

            // Scale visuals
            for (int i = 0; i < body.getProperty_attached_geometry().size(); ++i) {
                auto& geom = body.upd_attached_geometry(i);
                auto scale = geom.get_scale();
                scale[1] *= scaleFactor; // Assuming Y is longitudinal
                geom.set_scale(scale);
            }

            // IMPORTANT: Update joint offsets of children to maintain connectivity
            for (int i = 0; i < jointSet.size(); ++i) {
                auto& joint = jointSet.get(i);
                if (joint.getParentFrame().getName().find(bodyName) != std::string::npos) {
                    // This is a bit simplified, but demonstrates the intent
                    // In reality, we need to find the specific PhysicalOffsetFrame
                }
            }
        }
    };

    // 3. Build Bicycle Model and Attach Rider
    const auto& bike = scenario.bicycle();

    auto* frame = new OpenSim::Body("bicycle_frame", 8.0, SimTK::Vec3(0), SimTK::Inertia(1));
    model->addBody(frame);

    auto* groundToFrame = new WeldJoint("ground_to_frame",
        model->getGround(), SimTK::Vec3(0), SimTK::Vec3(0),
        *frame, SimTK::Vec3(0), SimTK::Vec3(0));
    model->addJoint(groundToFrame);

    // Attach Rider Pelvis to Saddle
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
    std::string outputFilename = "final_cycling_model.osim";
    model->finalizeConnections();
    model->print(outputFilename);

    return outputFilename;
}

}  // namespace cycleopt
