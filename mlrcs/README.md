# ML-RCS Scheduling

Coursework implementation of multi-level resource-constrained scheduling for logic operations.

## Scope

- Heuristic scheduling path for resource-limited AND / OR / NOT operations.
- ILP-based scheduling path using Gurobi for comparison and reasoning.
- Public repository excludes benchmark circuits, official project documents, and grading data.

## Build

This project currently expects a C++11 compiler and Gurobi headers/libraries for the ILP path.

```bash
make
```

Set `GUROBI_HOME` before building if using the provided Makefile:

```bash
export GUROBI_HOME=/path/to/gurobi
```

Gurobi is not bundled. Users must provide their own installation and license.

## Usage

```bash
./mlrcs -h INPUT.blif AND_LIMIT OR_LIMIT NOT_LIMIT
./mlrcs -e INPUT.blif AND_LIMIT OR_LIMIT NOT_LIMIT
```

- `-h`: heuristic scheduling
- `-e`: ILP-based scheduling

## Notes

This is an academic-scale coursework implementation. The public version is intended to show scheduling formulation and implementation structure without redistributing unauthorized benchmark files.
