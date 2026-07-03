# cycleOpt System Structure

## Overview
このドキュメントは、cycleOpt の主要なデータ構造とコンポーネントの関係を説明します。

## ドメインモデル

- `SubjectModel`
  - 名前: `name`
  - 体重 (kg): `body_mass_kg`
  - 身長 (m): `height_m`
  - 座高 (m): `seated_height_m`
  - 前腕長 (m): `forearm_length_m`
  - 上腕長 (m): `upper_arm_length_m`
  - 大腿長 (m): `thigh_length_m`
  - 下腿長 (m): `shank_length_m`
  - 首長 (m): `neck_length_m`

- `Point3D`
  - x: `x`
  - y: `y`
  - z: `z`

- `BicycleGeometry`
  - クランク長 (m): `crank_length_m`
  - サドル高 (m): `saddle_height_m`
  - ハンドル高 (m): `handlebar_height_m`
  - サドル位置: `saddle_position` (`Point3D`)
  - BB位置: `bottom_bracket_position` (`Point3D`)
  - ハンドル位置: `handlebar_position` (`Point3D`)
  - Q factor (m): `q_factor_m`
  - ハンドル幅 (m): `handlebar_width_m`

- `CyclingCondition`
  - 出力 (W): `power_watts`
  - ケイデンス (rpm): `cadence_rpm`
  - 速度 (m/s): `speed_mps`

- `RidingStyle`
  - 種類: `type`
  - 筋肉重み: `muscle_weight`
  - 代謝重み: `metabolic_weight`
  - 体積重み: `volume_weight`

- `SimulationScenario`
  - `subject` (`SubjectModel`)
  - `bicycle` (`BicycleGeometry`)
  - `cycling_condition` (`CyclingCondition`)
  - `riding_style` (`RidingStyle`)

## 追加設計

- OpenSim 連携は `opensim_adapter` で抽象化し、`OpenSimModelFactory` と `MocoProblemTranslator` でモデル生成と Moco 問題定義を分離する。
- `ObjectiveFunction` 抽象クラスを導入し、現在は以下の目的関数を実装予定。
  - `MuscleActivityObjective`
  - `MetabolicEnergyObjective`
  - `VolumeWeightedActivityObjective`
- `OptimizationService` は複数の `ObjectiveFunction` を受け取り、評価スコアを返す構造。
- 現在は JSON 入力を使い、最適化は `SimulationScenario` から問題構築までのパイプラインを想定。

## アプリケーション層

- `OptimizationService`
  - `SimulationScenario` を受け取り、最適化を実行して結果文字列を返す

## インフラ層

- `JsonInputLoader`
  - JSON ファイルから `SimulationScenario` を読み込む

- `ResultExporter`
  - 最適化結果をファイルへ書き出す

## インターフェース

- `main.cpp`
  - CLI エントリポイント
  - 入力 JSON ファイルを受け取り、最適化を実行し、結果を出力する

## DBML での構造

- `docs/cycleopt_structure.dbml` に、ドメインモデルをテーブルとして定義
- `simulation_scenario` は各ドメインエンティティを参照する形で構成される

## 参考
- `docs/json_input_spec.md`: JSON 入力形式の仕様
- `include/cycleopt/application/objective_function.hpp`: 目的関数抽象インターフェース
- `include/cycleopt/infrastructure/open_sim_model_factory.hpp`: OpenSim モデル生成ファクトリ設計
- `include/cycleopt/infrastructure/moco_problem_translator.hpp`: Moco 問題翻訳設計

- `include/cycleopt/domain/` にドメインクラス定義
- `include/cycleopt/application/optimization_service.hpp` にオプティマイズ処理定義
- `include/cycleopt/infrastructure/` に入出力インターフェース定義

