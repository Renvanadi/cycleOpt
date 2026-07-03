# cycleOpt architecture

This project is intentionally structured to support future research extensions while keeping the initial implementation simple.

## Design goals
- Minimize muscle activity, metabolic energy, and weighted muscle activity based on muscle volume.
- Accept input as JSON.
- Implement everything in C++.
- Keep the system extensible for future features such as video comparison, aerodynamics, and dancing.

## Layers
- Domain: models and concepts for subjects, bicycle geometry, conditions, and riding style.
- Application: orchestration of optimization requests.
- Infrastructure: JSON input loading and result export.
- Interface: CLI entry point.

## Extension points
- `ObjectiveFunction` を導入し、筋活動・代謝・体積重みの目的関数を追加できる。
- 将来的に新しい目的関数や制約を `OptimizationService` に追加しやすい構造。
- `OpenSimModelFactory` と `MocoProblemTranslator` で OpenSim / Moco の役割を分離し、実装変更に強い。
- JSON 入力仕様書は `docs/json_input_spec.md` にまとめている。
- クラス構造は `docs/cycleopt_structure.dbml` に定義している。
