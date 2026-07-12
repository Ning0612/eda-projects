# Clock Tree Synthesis

Coursework implementation of a clock tree synthesis flow.

![Clock tree example](../assets/cts-clock-tree.png)

The green point marks the clock source. Purple points are clock sinks. The black rectilinear segments show the generated clock-tree topology.

## Algorithm

The flow reads a source point, sink points, and core dimensions, then builds a rectilinear tree and reports the result as line segments. `Point` is the integer grid coordinate primitive, `Line` stores horizontal or vertical routing segments and provides distance/intersection helpers, and `ClockTree` owns the source/sink set, generated edges, skew metrics, and candidate construction strategies.

`tryBest()` uses a fixed fallback sequence rather than a global optimizer. It first evaluates an H-tree-like recursive partitioning candidate. If that candidate is invalid, it rebuilds and evaluates a FLUTE-based rectilinear Steiner tree candidate. If FLUTE is also invalid, it falls back to a simpler brute-force tree. FLUTE supplies compact low-wirelength topology support, while the H-tree-like candidate is used as the first skew-oriented construction attempt.

The core trade-off is wirelength versus skew. FLUTE tends to reduce rectilinear wirelength, but a wirelength-oriented Steiner tree can produce uneven source-to-sink path lengths. The H-tree-like path is more skew-oriented but can spend more wire. The benchmark step therefore tracks minimum and maximum source-to-sink path distance, skew ratio, total routed length, and crossing validity. At coursework scale, the expensive steps are dominated by tree construction plus repeated shortest-path checks across generated segments; for `n` sinks and `e` line segments, validation is roughly proportional to `n` graph searches over `e` edges.

## Build

This public repository does not bundle FLUTE source or lookup-table data. To build the CTS project, obtain FLUTE from its original source and place the required files in this directory:

- `flute.cpp`
- `flute.h`
- `flute_int.h`
- `flute_malloc.h`
- `PORT9.dat`
- `POWV9.dat`

Then build:

```bash
g++ -std=c++11 cts.cpp ClockTree.cpp Line.cpp Point.cpp flute.cpp -o cts
```

## Usage

```bash
./cts INPUT_FILE OUTPUT_FILE
```

See `examples/tiny.cts` for a small synthetic input. It is hand-written for public smoke testing and does not come from any official benchmark.

## Input format

The program accepts one whitespace-delimited file. The metadata tokens may appear before the point list in any order:

```text
.p <point_count>
.dimx <core_width>
.dimy <core_height>
<source_x> <source_y>
<sink_1_x> <sink_1_y>
...
<sink_(point_count-1)_x> <sink_(point_count-1)_y>
```

`.p` is the total number of points, including the source. The first coordinate pair is the clock source and the remaining `point_count - 1` pairs are sinks; coordinates and dimensions are parsed as integers. The parser only consumes these values, so no benchmark-specific footer is required. `examples/tiny.cts` is the smallest public example.

The repository also includes a tiny synthetic case generator derived from the original coursework utility, cleaned so generated cases can be written outside the repository. Pass an explicit seed for reproducible output:

```bash
g++ -std=c++11 examples/case_gen.cpp -o case_gen
./case_gen 100 100 8 /tmp/generated.cts 1
```

Benchmark inputs and official course materials are intentionally not included in the public repository.

## FLUTE Attribution

This project was developed against FLUTE, originally by Dr. Chris C. N. Chu, Iowa State University, with later public-use modifications commonly distributed with attribution in the source headers. Because redistribution terms must be verified separately, this cleaned public repository does not include FLUTE source or lookup-table data.
