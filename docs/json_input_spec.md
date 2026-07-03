# JSON 入力仕様

このドキュメントは cycleOpt の入力 JSON 形式を定義します。

## ルート構造

```json
{
  "name": "rider name",
  "body_mass_kg": 70.0,
  "height_m": 1.75,
  "seated_height_m": 0.9,
  "forearm_length_m": 0.3,
  "upper_arm_length_m": 0.3,
  "thigh_length_m": 0.45,
  "shank_length_m": 0.45,
  "neck_length_m": 0.15,
  "bicycle": {
    "crank_length_m": 0.17,
    "saddle_height_m": 0.85,
    "handlebar_height_m": 1.0,
    "saddle_position": {"x": 0.0, "y": 0.0, "z": 0.0},
    "bottom_bracket_position": {"x": 0.0, "y": 0.0, "z": 0.0},
    "handlebar_position": {"x": 0.0, "y": 0.0, "z": 0.0},
    "q_factor_m": 0.15,
    "handlebar_width_m": 0.4
  },
  "cycling_condition": {
    "power_watts": 200.0,
    "cadence_rpm": 90.0,
    "speed_mps": 10.0
  },
  "riding_style": {
    "type": 0,
    "muscle_weight": 1.0,
    "metabolic_weight": 1.0,
    "volume_weight": 1.0
  }
}
```

## 項目詳細

- `name`: ライダー識別名
- `body_mass_kg`: 体重 [kg]
- `height_m`: 身長 [m]
- `seated_height_m`: 座高 [m]
- `forearm_length_m`: 前腕長 [m]
- `upper_arm_length_m`: 上腕長 [m]
- `thigh_length_m`: 大腿長 [m]
- `shank_length_m`: 下腿長 [m]
- `neck_length_m`: 首長 [m]

### bicycle
- `crank_length_m`: クランク長 [m]
- `saddle_height_m`: サドル高 [m]
- `handlebar_height_m`: ハンドル高 [m]
- `saddle_position`: サドル位置 (`Point3D`)
- `bottom_bracket_position`: BB位置 (`Point3D`)
- `handlebar_position`: ハンドル位置 (`Point3D`)
- `q_factor_m`: Q factor [m]
- `handlebar_width_m`: ハンドル幅 [m]

### cycling_condition
- `power_watts`: 出力 [W]
- `cadence_rpm`: ケイデンス [rpm]
- `speed_mps`: 速度 [m/s]

### riding_style
- `type`: `RidingStyleType` enum 値
- `muscle_weight`: 筋活動重み
- `metabolic_weight`: 代謝エネルギー重み
- `volume_weight`: 筋肉体積重み

## 備考

- 今後、`RidingStyleType` を列挙型で意味付きに置き換える予定。
- `riding_style.type` は現在 0, 1, 2 を許容し、それぞれ `EnergyEfficient`、`LargeMuscleGroup`、`UpperBodyFocus` に対応する。
