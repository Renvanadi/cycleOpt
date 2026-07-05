/**
 * test_scaling.cpp
 *
 * 目的: Model::scale() APIを用いたセグメントの長さと質量の変更方法を学ぶ。
 */
#include <OpenSim/OpenSim.h>

using namespace OpenSim;

int main() {
    try {
        // テスト用の簡単なモデルを作成
        Model model;
        auto* rod = new Body("rod", 1.0, SimTK::Vec3(0), SimTK::Inertia(1));
        rod->attachGeometry(new Cylinder(0.05, 0.5)); // 半径0.05, 長さ0.5
        model.addBody(rod);

        auto* jnt = new PinJoint("jnt", model.getGround(), SimTK::Vec3(0), SimTK::Vec3(0),
                                 *rod, SimTK::Vec3(0, 0.25, 0), SimTK::Vec3(0));
        model.addJoint(jnt);
        model.finalizeConnections();

        // 1. スケーリングの設定 (ScaleSet)
        // rodを長さ方向(Y)に2倍にする設定
        ScaleSet scaleSet;
        auto* rodScale = new Scale();
        rodScale->setSegmentName("rod");
        rodScale->setScaleFactors(SimTK::Vec3(1.0, 2.0, 1.0)); // Y方向に2倍
        rodScale->setApply(true);
        scaleSet.adoptAndAppend(rodScale);

        // 2. スケーリングの実行
        // システムを初期化してStateを取得
        SimTK::State& state = model.initSystem();

        std::cout << "Original mass: " << model.get_total_mass(state) << " kg\n";

        // 寸法のスケーリング (関節位置や筋肉付着点も自動調整される)
        model.scale(state, scaleSet, true);

        // 3. 質量のスケーリング
        // 全体質量を5kgに調整する
        double targetMass = 5.0;
        double currentMass = model.get_total_mass(state);
        model.scaleMass(state, targetMass / currentMass);

        std::cout << "Scaled mass: " << model.get_total_mass(state) << " kg\n";

        // 4. 保存
        model.print("learning_scaling.osim");
        std::cout << "Scaling test completed. Saved as: learning_scaling.osim\n";

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}
