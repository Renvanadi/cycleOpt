/**
 * test_moco_basics.cpp
 *
 * 目的: OpenSim Moco を用いた最適化問題の設定と解法（Solver）の基礎を学ぶ。
 */
#include <OpenSim/Moco/osimMoco.h>

using namespace OpenSim;

int main() {
    try {
        // 1. MocoStudyの作成
        MocoStudy study;
        study.setName("simple_moco_test");

        // 2. 最適化問題 (MocoProblem) の定義
        MocoProblem& problem = study.updProblem();

        // テスト用のダブルペンデュラムなどのモデルを想定
        // ここではAPIの呼び出し方を中心に記述
        Model model; // 本来は有効なosimファイルを読み込む
        // problem.setModelCopy(model);

        // 時間範囲の設定
        problem.setTimeBounds(0, 1.0);

        // 3. 目的関数 (Goals) の追加
        // 筋活動や努力量を最小化する
        auto* effort = problem.addGoal<MocoControlGoal>("effort");
        effort->setWeight(10.0);

        // 周期性の設定
        auto* periodicity = problem.addGoal<MocoPeriodicityGoal>("periodicity");
        // periodicity->addStatePair({"/jointset/hip/angle/value"});

        // 4. ソルバー (Solver) の設定
        // CasADiソルバーを使用（デフォルト）
        MocoCasADiSolver& solver = study.initCasADiSolver();
        solver.set_num_mesh_intervals(30); // メッシュの細かさ
        solver.set_optim_solver("ipopt"); // IPOPTを使用
        solver.set_optim_convergence_tolerance(1e-3);

        // 問題設定をファイルに出力（デバッグに便利）
        study.print("learning_moco_setup.omoco");

        std::cout << "Moco basics test completed. Setup saved to: learning_moco_setup.omoco\n";
        std::cout << "※実際にsolve()を実行するには、適切なアクチュエータを持つモデルの設定が必要です。\n";

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}
