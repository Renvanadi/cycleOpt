/**
 * test_model_building.cpp
 *
 * 目的: OpenSim でのボディ・ジョイント・ジオメトリ構築を学び、
 *       自転車の幾何情報を変えるだけでモデルを組み立てられるようにする。
 */
#include <OpenSim/OpenSim.h>

#include <iostream>
#include <string>

using namespace OpenSim;

namespace {

struct BikeGeometry {
    double crankLength = 0.1725;
    double saddleHeight = 0.74;
    double saddleSetback = 0.18;
    double reach = 0.45;
    double stack = 0.60;
    double wheelBase = 1.01;
    double wheelRadius = 0.34;
    double handleWidth = 0.42;
    double qFactor = 0.15;
};

Body* makeBody(const std::string& name, double mass) {
    SimTK::Vec3 com(0, 0, 0);
    SimTK::Inertia inertia(1.0, 1.0, 1.0);
    return new Body(name, mass, com, inertia);
}

void addFrameGeometry(Body& body, const BikeGeometry& geo) {
    body.attachGeometry(new Brick(SimTK::Vec3(geo.wheelBase * 0.55, 0.02, 0.02)));
    body.attachGeometry(new Brick(SimTK::Vec3(geo.stack * 0.45, 0.02, 0.02)));
    body.attachGeometry(new Brick(SimTK::Vec3(geo.reach * 0.45, 0.02, 0.02)));
    body.attachGeometry(new Cylinder(0.03, 0.12));
    body.attachGeometry(new Cylinder(0.03, 0.12));
}

void addWheelGeometry(Body& body, double radius) {
    body.attachGeometry(new Cylinder(radius, 0.02));
}

void addPedalGeometry(Body& body) {
    body.attachGeometry(new Brick(SimTK::Vec3(0.08, 0.02, 0.04)));
}

void addSaddleGeometry(Body& body, const BikeGeometry& geo) {
    body.attachGeometry(new Brick(SimTK::Vec3(geo.saddleSetback * 0.35, 0.02, 0.08)));
}

void addHandlebarGeometry(Body& body, const BikeGeometry& geo) {
    body.attachGeometry(new Brick(SimTK::Vec3(geo.handleWidth * 0.5, 0.02, 0.02)));
    body.attachGeometry(new Cylinder(0.02, 0.06));
}

}  // namespace

int main() {
    try {
        BikeGeometry geo;
        geo.crankLength = 0.1725;
        geo.saddleHeight = 0.74;
        geo.saddleSetback = 0.18;
        geo.reach = 0.45;
        geo.stack = 0.60;
        geo.wheelBase = 1.01;
        geo.wheelRadius = 0.34;
        geo.handleWidth = 0.42;
        geo.qFactor = 0.15;

        Model model;
        model.setName("DetailedBicycleModel");

        auto* frame = makeBody("frame", 12.0);
        auto* frontWheel = makeBody("front_wheel", 1.6);
        auto* rearWheel = makeBody("rear_wheel", 1.6);
        auto* crank = makeBody("crank", 0.8);
        auto* leftPedal = makeBody("left_pedal", 0.2);
        auto* rightPedal = makeBody("right_pedal", 0.2);
        auto* saddle = makeBody("saddle", 0.6);
        auto* handlebar = makeBody("handlebar", 0.6);

        addFrameGeometry(*frame, geo);
        addWheelGeometry(*frontWheel, geo.wheelRadius);
        addWheelGeometry(*rearWheel, geo.wheelRadius);
        addFrameGeometry(*crank, geo);
        addPedalGeometry(*leftPedal);
        addPedalGeometry(*rightPedal);
        addSaddleGeometry(*saddle, geo);
        addHandlebarGeometry(*handlebar, geo);

        model.addBody(frame);
        model.addBody(frontWheel);
        model.addBody(rearWheel);
        model.addBody(crank);
        model.addBody(leftPedal);
        model.addBody(rightPedal);
        model.addBody(saddle);
        model.addBody(handlebar);

        auto* groundToFrame = new WeldJoint(
            "ground_to_frame",
            model.getGround(), SimTK::Vec3(0), SimTK::Vec3(0),
            *frame, SimTK::Vec3(0), SimTK::Vec3(0));
        model.addJoint(groundToFrame);

        auto* frameToFrontWheel = new PinJoint(
            "frame_to_front_wheel",
            *frame, SimTK::Vec3(geo.wheelBase * 0.5, 0.0, 0.0), SimTK::Vec3(0),
            *frontWheel, SimTK::Vec3(0), SimTK::Vec3(0));
        model.addJoint(frameToFrontWheel);

        auto* frameToRearWheel = new PinJoint(
            "frame_to_rear_wheel",
            *frame, SimTK::Vec3(-geo.wheelBase * 0.5, 0.0, 0.0), SimTK::Vec3(0),
            *rearWheel, SimTK::Vec3(0), SimTK::Vec3(0));
        model.addJoint(frameToRearWheel);

        auto* frameToCrank = new PinJoint(
            "frame_to_crank",
            *frame, SimTK::Vec3(0.0, -0.05, 0.0), SimTK::Vec3(0),
            *crank, SimTK::Vec3(0), SimTK::Vec3(0));
        model.addJoint(frameToCrank);

        auto* crankToLeftPedal = new PinJoint(
            "crank_to_left_pedal",
            *crank, SimTK::Vec3(-geo.crankLength * 0.5, 0.0, 0.0), SimTK::Vec3(0),
            *leftPedal, SimTK::Vec3(0), SimTK::Vec3(0));
        model.addJoint(crankToLeftPedal);

        auto* crankToRightPedal = new PinJoint(
            "crank_to_right_pedal",
            *crank, SimTK::Vec3(geo.crankLength * 0.5, 0.0, 0.0), SimTK::Vec3(0),
            *rightPedal, SimTK::Vec3(0), SimTK::Vec3(0));
        model.addJoint(crankToRightPedal);

        auto* frameToSaddle = new WeldJoint(
            "frame_to_saddle",
            *frame, SimTK::Vec3(0.0, geo.saddleHeight * 0.5, 0.0), SimTK::Vec3(0),
            *saddle, SimTK::Vec3(0), SimTK::Vec3(0));
        model.addJoint(frameToSaddle);

        auto* frameToHandlebar = new WeldJoint(
            "frame_to_handlebar",
            *frame, SimTK::Vec3(geo.reach * 0.35, geo.stack * 0.35, 0.0), SimTK::Vec3(0),
            *handlebar, SimTK::Vec3(0), SimTK::Vec3(0));
        model.addJoint(frameToHandlebar);

        model.finalizeConnections();
        model.print("detailed_bicycle_model.osim");

        std::cout << "Detailed bicycle model completed successfully.\n";
        std::cout << "Saved as: detailed_bicycle_model.osim\n";
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}
