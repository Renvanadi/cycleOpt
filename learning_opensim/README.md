# Learning OpenSim API

このディレクトリには、cycleOptプロジェクトで使用されたOpenSimおよびMocoの主要なAPIの使い方を学ぶための学習用テストコードが含まれています。

## ファイル構成

1.  **test_model_building.cpp**:
    - `Body`の作成、`Geometry`の付着、`Joint`（WeldJoint, PinJoint）による接続方法を学びます。
2.  **test_scaling.cpp**:
    - `ScaleSet`と`Scale`オブジェクトを用いた寸法のスケーリング、および`scaleMass()`による質量調整を学びます。
3.  **test_moco_basics.cpp**:
    - `MocoStudy`, `MocoProblem`の定義、`MocoControlGoal`（努力量最小化）の設定、ソルバーの構成方法を学びます。

## ビルド方法

OpenSimがインストールされている環境で、以下の手順でビルドできます。

```bash
mkdir build
cd build
cmake ..
make
```

## 実行方法

```bash
./test_model_building
./test_scaling
./test_moco_basics
```

各プログラムを実行すると、それぞれのAPIの動作結果を反映した `.osim` ファイルや `.omoco` ファイルが生成されます。これらを OpenSim GUI で開いて確認することで、コードがどのようにモデルに影響を与えるかを視覚的に理解できます。
