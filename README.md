# Tile Placement Optimizer

This project implements a C-based solver for a combinatorial optimization problem. The goal is to place a set of tiles onto a grid (board) to maximize a score derived from color matching rules and value summation.

## Architecture

The solution utilizes a **Recursive Backtracking** algorithm to explore the state space of possible board configurations.

Key features:
* **Dynamic Memory Management:** Efficient handling of variable-sized boards and tile sets.
* **Constraint Satisfaction:** Respects pre-assigned (fixed) tiles on the input board.
* **Pointer Arithmetic:** Optimization to reference tiles without redundant data copying.

## Build

A Makefile is provided for compilation. Ensure you have `gcc` installed.

```bash
make
