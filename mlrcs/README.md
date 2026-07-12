# ML-RCS Scheduling

Coursework implementation of multi-level resource-constrained scheduling for logic operations.

## Scope

- Heuristic scheduling path for resource-limited AND / OR / NOT operations.
- ILP-based scheduling path using Gurobi for comparison and reasoning.
- Public repository excludes benchmark circuits, official project documents, and grading data.

## Algorithm

The scheduler parses a small BLIF-style logic network into `Gate` objects and a predecessor/successor adjacency list. `.inputs` become source nodes, `.outputs` connect to a synthetic `__END__` node, and `.names` blocks are classified as NOT, AND, or OR gates from their input count and truth-table line. Resource limits are tracked separately for AND, OR, and NOT operations.

The heuristic path is priority-based list scheduling. It first assigns each gate a priority by walking backward from outputs; gates with a longer downstream distance to an output receive larger priority values. During each control step, the scheduler collects gates whose predecessors are already scheduled, sorts ready gates by descending priority, and emits up to the resource limit for each operation type. This favors critical-path progress while preserving a simple deterministic implementation. Its runtime is roughly polynomial in the number of gates and edges, with sorting of ready gates dominating small coursework cases.

The ILP path models an exact comparison target. ASAP and ALAP passes bound the possible start time of each gate. Binary variables `x_gate,time` choose exactly one start time, dependency constraints force predecessors to start earlier than successors, resource constraints cap each operation type per time step, and the objective minimizes the scheduled time of `__END__`. This gives a clean optimality formulation, but requires Gurobi and can be slower than the heuristic on larger networks. In this public repo, the heuristic path builds without Gurobi; ILP mode is enabled only when built with `USE_GUROBI=1`.

## Build

The heuristic path only needs a C++11 compiler:

```bash
make
```

Build the optional ILP path with Gurobi:

```bash
export GUROBI_HOME=/path/to/gurobi
make USE_GUROBI=1
```

Gurobi is not bundled. Users must provide their own installation and license.

## Usage

```bash
./mlrcs -h INPUT.blif AND_LIMIT OR_LIMIT NOT_LIMIT
./mlrcs -e INPUT.blif AND_LIMIT OR_LIMIT NOT_LIMIT
```

- `-h`: heuristic scheduling
- `-e`: ILP-based scheduling

Synthetic examples:

```bash
./mlrcs -h examples/two_level.blif 1 1 1
./mlrcs -h examples/resource_pressure.blif 1 1 1
```

These files are hand-written toy circuits for public smoke testing and are not official benchmark cases.

## Input format

The scheduler accepts the small BLIF subset used by the public examples:

```text
.model <name>
.inputs <input_1> <input_2> ...
.outputs <output_1> <output_2> ...
.names <input_1> <input_2> ... <output>
<truth_table_row>
.end
```

`.inputs` and `.outputs` declare node names. In the currently supported public subset, each `.names` block declares one output and is followed by the single truth-table row consumed by the parser; additional truth-table rows are not modeled. The parser classifies one-input blocks as `NOT`, and multi-input rows containing `-` as `OR`, otherwise as `AND`. Continuation lines ending in `\\` are joined before parsing. The command-line resource limits are integers in `AND_LIMIT OR_LIMIT NOT_LIMIT` order, and the public examples show the expected complete invocation.

## Notes

This is an academic-scale coursework implementation. The public version is intended to show scheduling formulation and implementation structure without redistributing unauthorized benchmark files.
