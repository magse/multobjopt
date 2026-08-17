# Repository guidance for coding agents

## Scope

These instructions apply to the entire repository. `multobjopt` is a
dependency-free C++20 library for bounded, constrained, multi-objective
engineering optimization. Preserve that small, portable library surface when
making changes.

## Project layout

- `include/multobjopt/multobjopt.hpp` is the self-contained umbrella API.
  Focused public headers separate fundamental types, problem construction,
  options, results, evaluation helpers, optimization entry points, and the
  generated Git version metadata.
- Public problem, option, evaluation, selection, dispatch, and enum-formatting
  operations each have a focused snake-case source file under `src/`. Overloads
  of one logical method, such as `problem::add_parameter`, stay together.
- Evaluator methods and shared optimizer operations each have focused source
  files; `src/detail/optimizer_detail.hpp` declares their internal contracts.
- Each optimization algorithm has one focused entry-point source file. Reusable
  algorithm calculations have matching support sources and private headers in
  `src/detail/`.
- `examples/` contains short executable demonstrations. It can be built either
  as part of this repository or against an installed `multobjopt` package. The
  current examples are an initial set intended to grow with focused engineering
  use cases.
- `tests/` contains plain, independently compiled CTest programs and the
  installed-package consumer test.
- `docs/` and `Doxyfile.in` provide the generated API documentation.
- `cmake/` contains package configuration files used by `find_package`.
  `generate_version_header.cmake` refreshes the installed public version header.

Do not edit generated files under `build*`, `cmake-build-*`, or Doxygen output
directories.

## Behavioral contracts

- A restriction value greater than or equal to zero is feasible. A negative
  value is a violation; do not invert this convention.
- Overall validation runs after all objective and restriction callbacks and
  receives their raw values in declaration order.
- Raw objective values remain available in results. Objective sense controls
  dominance, while positive objective weights select the scalar compromise.
- Feasible designs always rank ahead of infeasible designs. Only feasible,
  nondominated designs belong in the Pareto archive.
- Parameter grids are anchored at `lower_bound`. A positive resolution admits
  `lower_bound + k * resolution` without exceeding `upper_bound`; an unaligned
  upper bound is not an additional grid point.
- One evaluation means one complete design evaluation, irrespective of the
  number of callbacks. No algorithm may exceed `max_evaluations`.
- Fixed seeds, stable tie-breaking, and deterministic result ordering are part
  of the testable behavior.
- Automatic algorithm selection is a documented structural heuristic. Keep
  `select_algorithm()` and the README policy table consistent.

## Code and documentation style

- Use C++20 and snake_case for files, types, functions, variables, and enum
  values. Conventional upper-case include guards and CMake option names are
  acceptable.
- Keep the library free of third-party runtime dependencies.
- Add Doxygen comments for every public type, function, option, and result
  field, and focused `@file`/function documentation in implementation units.
  Use ordinary comments for local numerical reasoning and algorithmic
  invariants that are not obvious from the code.
- Keep each public method, evaluator method, optimizer entry point, and reusable
  algorithm operation in its own focused implementation file. Overloads of the
  same logical method and tiny helpers used only by one operation may remain
  together.
- Format C++ with the repository `.clang-format` file.
- Avoid changing public semantics silently. Update the README, INSTALL guide,
  examples, and tests when a public contract changes.

## Build and verification

Use an out-of-source build:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

For focused library-only work:

```sh
cmake -S . -B build-library \
  -DMULTOBJOPT_BUILD_TESTS=OFF \
  -DMULTOBJOPT_BUILD_EXAMPLES=OFF
cmake --build build-library --parallel
```

When adding tests:

- Use the next numbered filename, such as `34_test_new_contract.cpp`.
- Keep one self-contained `main()` per test and use `test_support.hpp`; do not
  add a test framework.
- Use fixed random seeds and assert invariants or tolerant quality thresholds,
  not an exact stochastic trajectory.
- Add the test to `tests/CMakeLists.txt` with an appropriate CTest label and a
  short timeout.
- Cover bounds, resolution, feasibility, evaluation accounting, and
  determinism whenever an optimizer changes.

Before handing off changes, also verify installation/package consumption when
the CMake export surface was touched:

```sh
ctest --test-dir build --output-on-failure -R 25_test_install_package
```
