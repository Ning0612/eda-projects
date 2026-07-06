# Clock Tree Synthesis

Coursework implementation of a clock tree synthesis flow.

## Build

```bash
g++ -std=c++11 cts.cpp ClockTree.cpp Line.cpp Point.cpp flute.cpp -o cts
```

## Usage

```bash
./cts INPUT_FILE OUTPUT_FILE
```

Benchmark inputs and official course materials are intentionally not included in the public repository.

## FLUTE Attribution

This project uses FLUTE, originally by Dr. Chris C. N. Chu, Iowa State University, with later public-use modifications credited in the source headers. Keep the FLUTE attribution and license notes when reusing this code.
