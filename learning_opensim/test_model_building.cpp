/**
 * test_model_building.cpp
 *
 * 目的: OpenSimでのモデル構築（Bodyの作成とJointでの接続）の基本を学ぶ。
 */
#include <OpenSim/OpenSim.h>

using namespace OpenSim;

int main() {
    try {
        // 1. モデルの初期化
        Model model;
        model.setName("SimpleBicycleModel");

        // 2. Body（剛体）の作成
        // Body(名前, 質量, 質量中心のオフセット, 慣性モーメント)
        double frameMass = 10.0;
        SimTK::Vec3 com(0, 0, 0);
        SimTK::Inertia inertia(1, 1, 1);
        auto* bikeFrame = new Body("bike_frame", frameMass, com, inertia);

        // 見た目（Geometry）の追加
        bikeFrame->attachGeometry(new Brick(SimTK::Vec3(0.5, 0.02, 0.02)));
        model.addBody(bikeFrame);

        // 3. Joint（関節）の作成と接続
        // WeldJoint: 完全に固定する接続
        // WeldJoint(名前, 親Frame, 親側オフセット, 親側向き, 子Frame, 子側オフセット, 子側向き)
        auto* groundJoint = new WeldJoint("ground_to_bike",
            model.getGround(), SimTK::Vec3(0, 1.0, 0), SimTK::Vec3(0),
            *bikeFrame, SimTK::Vec3(0), SimTK::Vec3(0));
        model.addJoint(groundJoint);

        // PinJoint: 1自由度の回転（クランクなど）
        auto* crank = new Body("crank", 0.5, com, SimTK::Inertia(0.01));
        model.addBody(crank);

        auto* bbJoint = new PinJoint("bottom_bracket",
            *bikeFrame, SimTK::Vec3(0, -0.5, 0), SimTK::Vec3(0),
            *crank, SimTK::Vec3(0), SimTK::Vec3(0));
        model.addJoint(bbJoint);

        // 4. モデルの完成と保存
        model.finalizeConnections();
        model.print("learning_model_building.osim");

        std::cout << "Model building test completed successfully.\n";
        std::cout << "Saved as: learning_model_building.osim\n";

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}
