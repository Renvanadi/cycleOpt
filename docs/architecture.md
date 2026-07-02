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
- Objective functions can be replaced by introducing new implementations.
- Constraints and optimization settings can be added later.
- External OpenSim logic will be integrated behind the application layer.
