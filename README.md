# EDA Projects

Cleaned public versions of three EDA/CAD coursework projects:

> **課程專案**：國立臺灣科技大學｜113.1｜CS5140701｜電腦輔助晶片系統設計｜三項課程作業合併

- `cts/` - Clock Tree Synthesis
- `legalization/` - Placement Legalization
- `mlrcs/` - Multi-level Resource-Constrained Scheduling

> Coursework, cleaned public version. These are academic-scale implementations for learning and interview discussion, not production EDA tools.
> These projects come from 113.1 NTUST CS5140701 Computer-Aided Design for VLSI Systems coursework. The public cleanup is complete; this repository is not planned for ongoing maintenance.

## Public Release Boundary

This repository intentionally does **not** redistribute:

- official course specifications or slides
- benchmark suites, hidden cases, or grading data
- submission archives
- scores, ranks, or private grading feedback
- third-party source/data files whose redistribution terms are unclear

Only self-written source code, attribution notes, and high-level problem descriptions are included. Any result discussion should use sanitized summaries that do not reconstruct unauthorized benchmark content.

## Projects

| Directory | Problem | Implementation Notes |
|---|---|---|
| `cts/` | Build a clock tree connecting sinks while controlling wirelength/skew-related structure. | C++ implementation designed to use FLUTE for rectilinear Steiner tree estimation/routing support. FLUTE source and LUT files are not bundled; see `cts/README.md`. |
| `legalization/` | Move cells from an initial placement into legal row/subrow positions while minimizing displacement. | C++ implementation organized around rows, subrows, components, and placement parsing. |
| `mlrcs/` | Schedule logic operations under resource constraints. | C++ implementation with heuristic scheduling and an ILP path using Gurobi. Gurobi is not bundled; users must provide their own installation/license. |

## Visuals

The three projects map to different points around the early physical-design flow. This is a simplified orientation diagram, not a claim that the repository implements every stage:

![Simplified physical-design flow showing ML-RCS, placement legalization, and CTS](assets/physical-design-flow.svg)

| Project | Example |
|---|---|
| Clock Tree Synthesis | ![Clock tree example with green clock source and purple clock sinks](assets/cts-clock-tree.png) |
| Placement Legalization | ![Placement before and after legalization](assets/legalization-before-after.png) |

## Build Notes

Each project keeps its own minimal build instructions in its subdirectory. Toolchains were developed for a Unix-like C++ environment; Windows users may need WSL2 or MSYS2.

The repository includes a GitHub Actions smoke workflow that builds the public-dependency paths, runs synthetic placement and ML-RCS toy cases, and compiles the CTS sources that do not require FLUTE. The toy cases are hand-written and intentionally separate from any official benchmark data.

## License

This cleaned public version is released under the MIT License. Third-party tools and external dependencies used by the coursework remain under their own licenses and are not redistributed here.
