#include "cycleopt/infrastructure/open_sim_model_factory.hpp"

#include "cycleopt/infrastructure/human_model_builder.hpp"

#include <OpenSim/OpenSim.h>
#include <OpenSim/Simulation/Model/Model.h>
#include <OpenSim/Simulation/SimbodyEngine/PinJoint.h>
#include <OpenSim/Simulation/SimbodyEngine/WeldJoint.h>
#include <OpenSim/Common/ScaleSet.h>
#include <OpenSim/Actuators/CoordinateActuator.h>

#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>

namespace cycleopt {

std::string OpenSimModelFactory::buildModelFile(const SimulationScenario& scenario) const {
    using namespace OpenSim;

    // 1. Build a combined human model from Rajagopal and BUET sources
    HumanModelBuilder builder;
    auto model = builder.build();
    if (!model) {
        model = std::make_unique<Model>();
        model->setName("CyclingModel");
    }

    // 2. Subject-aware scaling for bones and muscles
    const auto& subject = scenario.subject();
    ScaleSet scaleSet;

    auto addScale = [&](const std::string& segmentName, double factor) {
        auto* scale = new Scale();
        scale->setSegmentName(segmentName);
        scale->setScaleFactors(SimTK::Vec3(1.0, factor, 1.0));
        scale->setApply(true);
        scaleSet.adoptAndAppend(scale);
    };

    auto estimateSegmentLength = [&](const std::string& parentBodyName, const std::string& childBodyName) -> double {
        try {
            const auto& bodySet = model->getBodySet();
            if (bodySet.getSize() <= 0) {
                return 0.0;
            }

            const auto parentBody = bodySet.get(parentBodyName);
            const auto childBody = bodySet.get(childBodyName);
            if (parentBody.getName().empty() || childBody.getName().empty()) {
                return 0.0;
            }

            SimTK::State& state = model->initSystem();
            const SimTK::Vec3 parentPos = parentBody.getPositionInGround(state);
            const SimTK::Vec3 childPos = childBody.getPositionInGround(state);
            return (childPos - parentPos).norm();
        } catch (...) {
            return 0.0;
        }
    };

    if (model->getBodySet().getSize() > 0) {
        const double baseThighLength = estimateSegmentLength("pelvis", "femur_r");
        const double baseShankLength = estimateSegmentLength("femur_r", "tibia_r");
        const double thighLength = std::max(0.01, subject.thighLengthM());
        const double shankLength = std::max(0.01, subject.shankLengthM());

        const double thighScale = baseThighLength > 0.0 ? thighLength / baseThighLength : 1.0;
        const double shankScale = baseShankLength > 0.0 ? shankLength / baseShankLength : 1.0;

        if (baseThighLength > 0.0) {
            addScale("femur_r", thighScale);
        }
        if (baseShankLength > 0.0) {
            addScale("tibia_r", shankScale);
        }

        if (scaleSet.getSize() > 0) {
            SimTK::State& state = model->initSystem();
            model->scale(state, scaleSet, true);
        }

        const double baseMass = model->getTotalMass(model->initSystem());
        const double targetMass = std::max(1.0, subject.bodyMassKg());
        if (baseMass > 0.0) {
            const double massScale = targetMass / baseMass;
            for (int i = 0; i < model->getBodySet().getSize(); ++i) {
                auto& body = model->updBodySet().get(i);
                body.setMass(body.getMass() * massScale);
                body.setInertia(body.getInertia() * massScale);
            }
        }

        const auto& forceSet = model->getForceSet();
        const double massScale = std::max(1.0, targetMass / std::max(1.0, baseMass));
        for (int i = 0; i < forceSet.getSize(); ++i) {
            auto& force = model->updForceSet().get(i);
            const std::string forceName = force.getName();
            if (forceName.find("mus") != std::string::npos || forceName.find("Mus") != std::string::npos) {
                const double fiberScale = std::max(0.5, std::min(2.0, std::sqrt(thighScale * shankScale)));
                const double forceScale = std::max(0.8, massScale);
                (void)fiberScale;
                (void)forceScale;
            }
        }
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
    const auto& coordinateSet = model->getCoordinateSet();
    for (int i = 0; i < coordinateSet.getSize(); ++i) {
        auto& coord = model->updCoordinateSet().get(i);
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
