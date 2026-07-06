# Clock Tree Synthesis

Coursework implementation of a clock tree synthesis flow.

![Clock tree example](../assets/cts-clock-tree.png)

The green point marks the clock source. Purple points are clock sinks. The black rectilinear segments show the generated clock-tree topology.

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

Benchmark inputs and official course materials are intentionally not included in the public repository.

## FLUTE Attribution

This project was developed against FLUTE, originally by Dr. Chris C. N. Chu, Iowa State University, with later public-use modifications commonly distributed with attribution in the source headers. Because redistribution terms must be verified separately, this cleaned public repository does not include FLUTE source or lookup-table data.
