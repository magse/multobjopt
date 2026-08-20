# multobjopt

> **Project status and engineering disclaimer:** `multobjopt` is an early-stage
> experimental library. Its algorithms do not guarantee a global optimum or
> the correctness or safety of a resulting design. Independently verify all
> models, constraints, and optimization results before using them in production
> or safety-critical applications.

> **AI-assisted development:** This library was created with extensive help
> from OpenAI Codex using the GPT model `gpt-5.6-sol` at the `ultra`
> reasoning-effort level.

`multobjopt` is a dependency-free C++20 library for bounded, constrained,
multi-objective engineering optimization. A problem is assembled from scalar
parameters, objective functions, restriction functions, and an optional final
validation function. The same problem can be solved with simulated annealing,
a genetic algorithm, Box's complex method, or gradient descent using numerical
gradients. The library can also select a method automatically.

A distinctive part of the problem model is the coupled overall validator. It
runs after all scalar objectives and restrictions for a candidate are known and
can make one final system-level acceptance decision from that complete output
state. This is useful for assembly compatibility and cross-result rules that do
not have a natural scalar violation distance.

The library is intended for designing mechatronic machinery, such as robot
arms.

It is also designed to be used alongside
[NURBSpath](https://github.com/magse/NURBSpath), primarily on Linux and macOS
systems.

## Quick start

```cpp
#include <multobjopt/multobjopt.hpp>

#include <iostream>

int main() {
    using namespace multobjopt;

    problem design;
    design
        .add_parameter("x", -5.0, 5.0)
        .add_parameter("y", -5.0, 5.0)
        .add_objective(
            "error",
            objective_sense::minimize,
            [](scalar_view values) {
                const auto dx = values[0] - 1.5;
                const auto dy = values[1] + 0.5;
                return dx * dx + dy * dy;
            })
        .add_restriction(
            "inside_circle",
            [](scalar_view values) {
                return 9.0 - values[0] * values[0] - values[1] * values[1];
            });

    optimizer_options options;
    options.algorithm = optimization_algorithm::automatic;
    options.max_evaluations = 4'000;

    const auto result = optimize(design, options);
    std::cout << result.best_design.parameters[0] << ", "
              << result.best_design.parameters[1] << '\n';
}
```

The problem owns its callbacks, so ordinary functions, lambdas, and copyable
stateful functors all work. Lambdas may capture a shared simulation model when
several outputs come from one engineering analysis.

## Public headers

Most applications should include the umbrella header used above:

```cpp
#include <multobjopt/multobjopt.hpp>
```

Smaller components can include a focused, self-contained header instead:

| Header | Contents |
| --- | --- |
| `config.hpp` | Generated semantic-version and Git-revision macros |
| `types.hpp` | Scalar aliases, callback types, enums, and parameter/objective/restriction definitions |
| `problem.hpp` | Fluent problem construction, stored definitions, and validation |
| `options.hpp` | Common and method-specific optimizer settings |
| `result.hpp` | Owning evaluated-design and optimization-result records |
| `evaluation.hpp` | Design normalization, direct evaluation, and Pareto dominance |
| `optimize.hpp` | Automatic selection, optimization dispatch, and enum names |
| `reporting.hpp` | Opt-in CSV/TSV evaluation and Pareto writers plus JSON summaries |
| `version.hpp` | Type-safe C++ semantic version and Git revision metadata |

The umbrella header only includes these focused headers; it adds no second set
of declarations. Every public header has detailed Doxygen documentation and can
be compiled independently.

The generated config header exposes the numeric
`MULTOBJOPT_VERSION_MAJOR`, `MULTOBJOPT_VERSION_MINOR`, and
`MULTOBJOPT_VERSION_PATCH` macros, the stable `MULTOBJOPT_VERSION_STRING`, and
the descriptive `MULTOBJOPT_LIBRARY_VERSION`. It also provides
`MULTOBJOPT_VERSION_FROM_GIT`, `MULTOBJOPT_GIT_DESCRIPTION`,
`MULTOBJOPT_GIT_COMMIT`, `MULTOBJOPT_GIT_BRANCH`, and
`MULTOBJOPT_GIT_DIRTY`; the two flags are literal `0` or `1` for use in
preprocessor conditions. `MULTOBJOPT_SEMANTIC_VERSION` is a descriptive alias
for `MULTOBJOPT_VERSION_STRING`.

The version header mirrors those values as `multobjopt::version` constants.
When the project root contains a Git checkout with at least one commit,
`library_version` and `MULTOBJOPT_LIBRARY_VERSION` are derived from
`git describe --tags --always`. Source archives, vendored copies without their
own `.git` entry, and repositories without a commit fall back to the CMake
project version.

## Problem semantics

### Construction and callback order

`problem` stores definitions in the order in which they are added. That order
is part of the model contract, not merely a presentation detail. During one
complete design evaluation the library:

1. clamps and quantizes parameters, preserving parameter declaration order;
2. calls every objective in objective declaration order;
3. calls every restriction in restriction declaration order; and
4. calls the optional overall validator once, after both output vectors are
   complete.

Objective and restriction callbacks receive normalized parameter values in
parameter declaration order. The validator receives the raw scalar outputs
exactly as returned by the objective and restriction callbacks, again in their
respective declaration order. Objective sense, objective weight, restriction
scale, and violation aggregation do not alter the values passed to it.

| Callback kind | Receives | Returns | Primary role |
| --- | --- | --- | --- |
| Objective | Normalized parameters | Scalar performance value | Quantity to minimize or maximize |
| Restriction | Normalized parameters | Scalar margin, feasible at `>= 0` | Requirement with a useful violation distance |
| Overall validator | All raw objective and restriction values | `bool` | Final coupled system or assembly acceptance |

For models with several indexed inputs and outputs, use named index constants
or enums beside the problem construction. This makes callback indexing
reviewable and reduces maintenance errors when a definition is inserted or
reordered:

```cpp
enum parameter_index : std::size_t {
    link_length_parameter,
    payload_parameter,
};
enum objective_index : std::size_t {
    mass_objective,
    cycle_time_objective,
};
enum restriction_index : std::size_t {
    torque_reserve_restriction,
    stress_reserve_restriction,
};

problem design;
design
    .add_parameter("link_length", 0.3, 1.2)
    .add_parameter("payload", 1.0, 20.0)
    // Objective indices follow this declaration order.
    .add_objective("mass", objective_sense::minimize,
                   [](scalar_view p) {
                       return 4.0 * p[link_length_parameter] +
                              0.2 * p[payload_parameter];
                   })
    .add_objective("cycle_time", objective_sense::minimize,
                   [](scalar_view p) {
                       return 0.5 + 0.04 * p[payload_parameter];
                   })
    // Restriction values are margins: zero or positive is feasible.
    .add_restriction("torque_reserve", [](scalar_view p) {
        return 30.0 - p[link_length_parameter] * p[payload_parameter];
    })
    .add_restriction("stress_reserve", [](scalar_view p) {
        return 80.0 - 5.0 * p[link_length_parameter] * p[payload_parameter];
    })
    .set_validation([](scalar_view objectives, scalar_view restrictions) {
        // These are raw outputs in the two declaration orders above. This
        // lookup-like assembly rule is a Boolean gate, not a distance.
        const bool catalog_drive_package_is_available =
            objectives[cycle_time_objective] <= 1.0
                ? restrictions[torque_reserve_restriction] >= 2.0
                : objectives[mass_objective] <= 7.0 &&
                      restrictions[stress_reserve_restriction] >= 5.0;
        return catalog_drive_package_is_available;
    });
```

Parameters have inclusive lower and upper bounds. A resolution of zero means a
continuous parameter. A positive resolution defines values on the lattice
`lower_bound + k * resolution`; if the upper bound is not on that lattice it
remains a bound and is not added as an irregular final point. Equal bounds make
a fixed continuous parameter. Designs generated by every optimizer are clamped
and quantized before callbacks run.

Each objective returns one finite scalar and declares whether it is minimized
or maximized. Its positive `weight` contributes to `scalarized_objective`, which
provides a single comparison score when an algorithm needs one. Objective
direction, rather than weight, defines Pareto dominance.
`optimization_result::pareto_front` therefore contains the feasible
nondominated designs discovered during any run.

Each restriction follows one convention:

```text
restriction(parameter_values) >= 0    feasible
restriction(parameter_values) <  0    violated
```

`optimizer_options::constraint_tolerance` permits an explicitly requested small
numerical boundary violation; its default is zero, which enforces the convention
strictly. Feasible designs always rank ahead of infeasible designs. Among
infeasible designs, the optimizer minimizes the sum of violation magnitudes.
Restrictions measured in different engineering units can set a positive
`scale`; each violation is divided by that scale before aggregation.

### Overall validation versus scalar restrictions

The coupled overall validator is a distinctive final layer above the ordinary
scalar callbacks. Use a scalar restriction when the model has a meaningful
margin or distance to acceptance. That magnitude helps rank and repair
infeasible designs. Use the optional overall validation function for a final
Boolean gate such as a coupled assembly rule, a permitted component
combination, controller/system compatibility, or acceptance logic involving
several already-computed objective and restriction outputs. It is especially
useful when rejection is clear but assigning a physically meaningful scalar
distance is artificial.

The validator can be added or replaced with `set_validation()`:

```cpp
design.set_validation(
    [](scalar_view objectives, scalar_view restrictions) {
        // Raw values, indexed in objective and restriction declaration order.
        return objectives[0] <= 12.0 && restrictions[1] >= 0.5;
    });
```

For an otherwise finite design, returning `false` leaves
`evaluated_design::valid` true, sets `feasible` to false, and adds one unit to
`total_violation` in addition to any scalar-restriction violation. The fixed
unit reflects that a Boolean gate provides no distance measure. The raw outputs
remain available for diagnosis, but the rejected design cannot enter the
feasible Pareto archive. Non-finite model outputs still make the design invalid
regardless of what the validator returns.

All objective and restriction callbacks run before validation unless one
throws, so validation does not short-circuit an expensive analysis. Prefer to
make the final gate inexpensive and base it on the output spans instead of
rerunning the simulation. One complete pass still counts as one design
evaluation, irrespective of the number or cost of its callbacks.

Callbacks and validators should be deterministic for a fixed input when
repeatable optimization is required. Avoid observable side effects and do not
make a later callback depend on mutation performed by an earlier callback;
evaluation-history-dependent models undermine fixed-seed reproducibility. A
cache or shared simulation object may be captured when useful, provided it
does not change the returned model values for the same design. The problem owns
copies of its callables, but none of them may retain a supplied `scalar_view`:
the view and its storage are valid only until that callback returns.

Non-finite callback values make a design invalid and infeasible, assigning it
an infinite score and violation. A callback may instead throw to report a
failed model evaluation; the exception propagates to the caller.

## Algorithm selection

Set `optimizer_options::algorithm` explicitly for repeatable method selection,
or leave it as `optimization_algorithm::automatic`. Automatic selection uses
the problem structure:

| Problem structure | Selected method |
| --- | --- |
| Multiple objectives or any quantized parameter | Genetic algorithm |
| Continuous problem with restrictions or a validator | Box method |
| Unconstrained continuous problem with at most 12 parameters | Gradient descent |
| Larger unconstrained continuous problem | Simulated annealing |

The genetic algorithm uses Pareto rank and crowding information for
multi-objective populations. Simulated annealing is useful for rough scalarized
landscapes. Box's complex method is derivative-free and handles bounded
constraints. Gradient descent uses projected numerical finite differences and
a backtracking line search.

All algorithm controls live in `optimizer_options`. `max_evaluations` counts
complete design evaluations, not individual scalar callback calls. Set
`random_seed` to reproduce stochastic runs. `initial_guess` is optional and is
normalized to the parameter bounds and resolutions.

`optimization_result` reports the actual `algorithm_used`, termination reason,
evaluation and iteration counts, the scalarized `best_design`, and the Pareto
archive. For a multi-objective run, prefer the Pareto front over treating
`best_design` as an intrinsically unique optimum. An empty front is reported as
`no_feasible_design`; `evaluated_design::valid` distinguishes a numerically
valid constraint violation from a failed/non-finite model evaluation.

## Optional run reports

`optimize()` does not open files or write to the console. Machine-readable
reporting is an explicit post-processing operation, so existing applications
produce no new output. Three independent writers are available:

| Writer | Contents |
| --- | --- |
| `write_evaluation_history()` | Every completed design evaluation in chronological order as CSV or TSV |
| `write_pareto_front()` | The final feasible nondominated archive as CSV or TSV |
| `write_summary_json()` | Run metadata, result counts, and the named best design as JSON |

Capturing every evaluation is also opt-in because a long run can retain much
more data than its final Pareto archive:

```cpp
#include <multobjopt/multobjopt.hpp>

#include <fstream>

multobjopt::optimizer_options options;
options.record_evaluation_history = true;

const auto result = multobjopt::optimize(problem_definition, options);

std::ofstream history{"run_evaluations.tsv"};
multobjopt::write_evaluation_history(
    history, problem_definition, result,
    multobjopt::delimited_text_format::tsv);

std::ofstream pareto{"run_pareto.tsv"};
multobjopt::write_pareto_front(
    pareto, problem_definition, result,
    multobjopt::delimited_text_format::tsv);

std::ofstream summary{"run_summary.json"};
multobjopt::write_summary_json(summary, problem_definition, result);
```

`record_evaluation_history` defaults to `false`. When enabled,
`result.evaluation_history[index]` is complete evaluation number `index + 1`
and `evaluation_history.size() == evaluations`. This is an evaluation trace,
not an algorithm-specific list of accepted moves or outer iterations: repeated
points, infeasible points, overall-validator rejections, and non-finite values
returned by callbacks are retained. A callback that throws does not complete
an evaluation and therefore cannot produce a row. Capturing history does not
change the evaluation budget, Pareto archive, ranking, or search decisions, but
its memory use grows with the number and size of evaluations.

Both table writers use definition names in their headers, normalized parameter
values, raw objective and restriction values, validity and feasibility flags,
aggregate violation, and the scalarized score. Rows use deterministic ordering
and round-trip scalar precision. CSV/TSV quoting protects delimiters, quotes,
and line breaks in user-defined names. The JSON summary is deterministic and
represents non-finite scalar values as `null`, because JSON has no NaN or
infinity number syntax.

The Pareto and JSON writers need only the ordinary optimization result. The
history writer additionally requires a result produced with history recording
enabled. Writers accept `std::ostream`, so the application retains control over
paths, overwrite policy, in-memory output, compression, and error handling;
call only the reports required for a particular run.

## Examples

> **Start with the worked examples guide:** [EXAMPLE.md](EXAMPLE.md) develops
> one minimal problem and one feature-rich mechatronic problem, including the
> coupled final-validation pattern and result interpretation.

The `examples` directory contains five focused API and engineering programs,
covering small smooth problems, Pareto search, mechatronic machinery design,
and optional machine-readable reporting:

| Example | Kind of problem | What it demonstrates |
| --- | --- | --- |
| `01_basic_quadratic.cpp` | Smooth, unconstrained, continuous minimization | Explicit gradient descent using numerical gradients, an initial guess, and recovery of the known optimum `(1.5, -0.5)` |
| `02_constrained_box.cpp` | Continuous sizing with a nonlinear restriction | Box's derivative-free complex method, the `restriction >= 0` convention, feasibility repair, and result inspection |
| `03_multiobjective_pareto.cpp` | Two conflicting continuous objectives | Pareto-aware genetic search, the nondominated archive, and sampling representative trade-offs instead of treating one compromise as the only answer |
| `04_mechatronic_discrete_auto.cpp` | Quantized mechatronic component selection | Gear, current, and arm parameters with resolutions; mixed minimize/maximize objectives; thermal, speed, and structural restrictions; coupled validation of approved product packages; and automatic selection of the genetic algorithm |
| `05_optional_reporting.cpp` | Small multi-objective reporting run | Default no-file behavior, optional evaluation-history capture, CSV or TSV trajectory and Pareto tables, and a JSON summary written after optimization |

The directory also contains 43 individually buildable transcriptions of the
[Wikipedia test-functions catalog](https://en.wikipedia.org/wiki/Test_functions_for_optimization).
Their requested `WP01_...` through `WP43_...` numbering follows the catalog's
display order:

| Files | Benchmark kind | Included functions |
| --- | --- | --- |
| `WP01_...`–`WP22_...` | Single-objective | Rastrigin through Shekel, including Ackley, Rosenbrock, Griewank, Himmelblau, Eggholder, and the other catalog entries |
| `WP23_...`–`WP26_...` | Constrained single-objective | Rosenbrock with disk constraint, Mishra Bird, Townsend, and Keane bump |
| `WP27_...`–`WP43_...` | Multi-objective, with and without constraints | Binh–Korn through Viennet, including Fonseca–Fleming, Kursawe, Schaffer, ZDT, Osyczka–Kundu, CTP1, and Constr-Ex |

Each benchmark keeps its objective and restriction formulas in its own source
file. A small shared runner provides deterministic settings, consistent output,
and, where the catalog publishes one, an independent reference-design check.
See [examples/wikipedia_benchmarks.md](examples/wikipedia_benchmarks.md) for the
complete file inventory and the explicitly documented dimension and bounded-
domain adaptations.

All examples use deterministic seeds and print the selected method plus the
most relevant design or Pareto values. They are deliberately small enough to
serve as starting points for user-provided simulation callbacks.

The current examples form a broad initial set, not a finished collection.
Future additions will cover more algorithms, modeling patterns, benchmark
families, and engineering design cases while keeping each example focused and
buildable.

The examples can be built with the repository, as shown below, or as a
standalone CMake project against an installed library. See
[INSTALL.md](INSTALL.md#build-the-examples-against-the-installed-package) for
the installed-library workflow.

## Build and test

A C++20 compiler and CMake 3.20 or newer are required.

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build --output-on-failure
```

Tests and examples are enabled by default when `multobjopt` is the top-level
project. They can be controlled independently:

```sh
cmake -S . -B build \
  -DMULTOBJOPT_BUILD_TESTS=ON \
  -DMULTOBJOPT_BUILD_EXAMPLES=ON
```

Run an example from a single-configuration build with, for example:

```sh
./build/examples/multobjopt_example_04_mechatronic_discrete_auto
```

## Install and consume

Detailed installation, package discovery, and standalone-example instructions
are in [INSTALL.md](INSTALL.md).

```sh
cmake --install build --prefix /path/to/prefix
```

A downstream CMake project can then use the installed package:

```cmake
find_package(multobjopt CONFIG REQUIRED)
target_link_libraries(my_design PRIVATE multobjopt::multobjopt)
target_compile_features(my_design PRIVATE cxx_std_20)
```

When the prefix is not in CMake's normal search path, configure the consumer
with `-DCMAKE_PREFIX_PATH=/path/to/prefix`.

## API documentation

The public headers contain Doxygen comments. Generate the HTML API reference
with an out-of-source build. Doxygen must be installed and discoverable by
CMake.

```sh
cmake -S . -B build-docs \
  -DMULTOBJOPT_BUILD_DOCUMENTATION=ON \
  -DMULTOBJOPT_BUILD_TESTS=OFF \
  -DMULTOBJOPT_BUILD_EXAMPLES=OFF
cmake --build build-docs --target docs
```

The `multobjopt_documentation` target remains available as the descriptive
project-specific name. Generated HTML starts at
`build-docs/docs/html/index.html`; the documentation source entry point is
`docs/mainpage.dox`.
