# Placement Legalization

Coursework implementation of standard-cell placement legalization.

![Placement before and after legalization](../assets/legalization-before-after.png)

Left: before legalization, cells are scattered and overlap. Right: after legalization, cells are aligned to rows and no longer overlap.

## Algorithm

The legalizer is a coursework-scale heuristic inspired by ideas similar to the FBM paper, "A Simple and Fast Algorithm for Placement Legalization." The paper itself is not bundled in this public repository. This is not a complete reproduction of the paper's full algorithm; the implementation focuses on fast row assignment, local whitespace balancing, overlap removal among placed movable cells, and displacement reporting rather than global optimal legalization.

It parses Bookshelf-style `.nodes`, `.pl`, and `.scl` files into `Component`, `CellRow`, and `SubRow` objects. `Component` stores original and legal positions plus dimensions, `CellRow` stores row orientation, height, site spacing, and parsed subrow metadata, and `Placement` coordinates parsing, assignment, overlap removal, and displacement reporting.

Placement starts by sorting movable cells by width, with larger cells placed earlier because they are harder to fit after row capacity is consumed. For each movable cell, the algorithm chooses the nearest compatible row with enough aggregate remaining space, snaps the cell to that row coordinate, and records row capacity usage. A balancing pass can move small cells from crowded rows toward neighboring rows with more free space. This is a local improvement step, not a global min-cost solver.

After row assignment, movable cells within each row are sorted by their legal coordinate. Overlaps among those placed cells are removed by shifting later cells to the end of the previous cell. If the row overflows, the algorithm shifts cells back using available gaps and then applies the remaining offset to keep the row inside its legal boundary. Fixed terminals are parsed and skipped as movable cells, but this public implementation does not reserve fixed-cell obstacle intervals during row assignment. The reported objective is total and maximum Euclidean displacement from original lower-left position to legal lower-left position. The practical complexity is dominated by sorting cells and rows plus local scans within each row; this keeps the implementation simple and deterministic for coursework-scale cases, while trading away global optimality.

## Build

```bash
g++ -std=c++11 CellRow.cpp Component.cpp Placement.cpp Point.cpp SubRow.cpp legalizer.cpp -o legalizer
```

## Usage

```bash
./legalizer INPUT_FILE OUTPUT_FILE
```

The input argument is a basename without extension. For example:

```bash
./legalizer examples/toy-overlap/input examples/toy-overlap/output
```

`examples/toy-overlap/` is a hand-written synthetic case with two overlapping movable cells and one fixed terminal. It is intended for smoke testing only.

The public version excludes benchmark suites, official course specifications, and grading data. It is intended to show the implementation structure and algorithmic approach at coursework scale.
