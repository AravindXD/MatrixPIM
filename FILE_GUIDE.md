# pPIM Compiler Project: File Guide

This document provides a brief explanation of each file in the project structure.

## Root Directory

- `Makefile`: Build configuration for compiling the entire project.
- `README.md`: Main documentation with project overview, architecture, and usage instructions.
- `output.asm`: Generated assembly output from the pPIM compiler for matrix multiplication.
- `cpu_benchmark`: Executable for benchmarking CPU matrix multiplication performance.
- `pim_simulator`: Executable for simulating pPIM assembly execution.
- `large_matrix_sim`: Executable for simulating large matrix multiplication performance.
- `Detailed Implementation Guide for Processing-in-Me.md`: Detailed implementation guidelines for the project.
- `Problem-Statement_B1-_B2-slot.pdf`: Original problem statement document.
- `Flexible_Instruction_Set_Architecture_for_Programmable_Look-up_Table_based_Processing-in-Memory.pdf`: Research paper describing the pPIM architecture.
- `ISA-for-pPIM.pptx.pdf`: Presentation about the pPIM instruction set architecture.

## src/ (Source Code)

- `src/main.cpp`: Entry point for the compiler, handles command-line arguments and workflow.
- `src/compiler.cpp`: Core compiler implementation that coordinates all compilation phases.
- `src/frontend/parser.cpp`: Parses C++ matrix code into intermediate representation.
- `src/memorymap/memorymap.cpp`: Maps matrix data to optimized memory layout for pPIM architecture.
- `src/optimizer/optimizer.cpp`: Implements optimization strategies for generated code.
- `src/backend/codegen.cpp`: Generates pPIM assembly code from optimized intermediate representation.
- `src/pim_isa/instructions.cpp`: Defines the pPIM instruction set and encoding.
- `src/utils/logger.cpp`: Logging utilities for debugging and verbose output.

## include/ (Header Files)

- `include/compiler.h`: Core compiler class definition and interfaces.
- `include/frontend/parser.h`: Parser class declaration and matrix representation structures.
- `include/memorymap/memorymap.h`: Memory mapping interfaces and address computation utilities.
- `include/optimizer/optimizer.h`: Optimization level definitions and optimizer interface.
- `include/backend/codegen.h`: Code generation classes and assembly pattern definitions.
- `include/pim_isa/instructions.h`: Instruction class definitions and encodings.
- `include/utils/logger.h`: Logging utility declarations and verbosity control.

## sim/ (Simulation)

- `sim/pim_simulator.cpp`: Simulates execution of pPIM assembly instructions.
- `sim/accurate_pim_sim.cpp`: Cycle-accurate simulator modeling memory and execution patterns.
- `sim/large_matrix_sim.cpp`: Specialized simulator for large matrix multiplication performance.

## scripts/ (Utilities)

- `scripts/generate_graphs.py`: Generates performance comparison graphs between CPU and pPIM.

## examples/ (Input Examples)

- `examples/matrix_multiplication.cpp`: Example C++ input file with matrix multiplication code.

## test/ (Testing)

- `test/cpu_benchmark.cpp`: Benchmarks traditional CPU matrix multiplication performance.
- `test/complex_test.cpp`: Tests for larger matrix multiplication scenarios.
- `test/test_matrix_mul.cpp`: Basic tests for matrix multiplication functionality.
- `test/test_main.cpp`: Test driver for the test suite.
- `test/output.asm`: Example assembly output for testing purposes.

## bin/ (Binaries)

- `bin/pim_compiler`: Compiled executable of the pPIM compiler.

## build/ (Build Artifacts)

- Contains object files and intermediate compilation outputs.

## graphs/ (Performance Visualization)

- `graphs/performance_log_scale.png`: Log-scale comparison of CPU vs pPIM execution times.
- `graphs/speedup.png`: Visualization of speedup factors at different matrix sizes.
- `graphs/combined_performance.png`: Combined performance metrics visualization.
- `graphs/large_matrices_comparison.png`: Performance comparison for large matrices.
- `graphs/performance_data.csv`: Raw performance data in CSV format.

## images/ (Documentation Images)

- Contains copies of performance graphs used in documentation. 